/* [Board dimensions] */
board_size = [40, 32]; 
board_clearance = .5;
cable_size = [3, 8.5];

/* [Top part] */
top_height = 25;

/* [Bottom part] */
bottom_height = 14;
bottom_lip = 4;

/* [Holder plate] */
holder_width = 50;
holder_hole_span = 43;
holder_hole_diameter = 2.5;
holder_nut_diameter = 5.5;

/* [Hidden] */
wth = 1.60;
wtv = .86;
outer_radius = 5;
vsig = ["PRUSACAM", "ESP32 BT/1.0", "RIDER.CZ 2021"];
$fudge = 1;

// Render all

translate([0, 100]) !bottom();
translate([0, 50]) top();
holder();

module holder() {
    holder_size = [
        board_size[0] + board_clearance + 4 * wtv, 
        holder_width, 
        wth
    ];

    bottom(hollow = true);

    difference() {
        // Plate
        translate([-holder_size[0] / 2, -holder_size[1] / 2]) rcube(holder_size, outer_radius, $fn = 32);

        // Screw holes
        for(y = [-holder_hole_span / 2, holder_hole_span / 2]) translate([0, y, -$fudge]) cylinder(d = holder_hole_diameter, h = 2 * wth + 2 * $fudge, $fn = 16);
    }

    // Nut holders    
    for(y = [-holder_hole_span / 2, holder_hole_span / 2]) translate([0, y, wth]) difference() {
        cylinder(d = holder_nut_diameter + wtv, h = wth, $fn = 6);
        translate([0, 0, -$fudge]) cylinder(d = holder_nut_diameter, h = wth + 2 * $fudge, $fn = 6);
    }
}

module top() {
    outer_size = [
        board_size[0] + board_clearance + 4 * wtv,
        board_size[1] + board_clearance + 4 * wtv,
        top_height + wth
    ];
    inner_size = [
        board_size[0] + board_clearance + 2 * wtv,
        board_size[1] + board_clearance + 2 * wtv,
        outer_size[2]
    ];
    inner_radius = outer_radius - wtv;

    // Main shape
    difference() {
        // Outer shape
        translate([-outer_size[0] / 2, -outer_size[1] / 2]) rcube(outer_size, outer_radius, $fn = 64);

        // Main hole
        translate([-inner_size[0] / 2, -inner_size[1] / 2, wth]) rcube(inner_size, inner_radius, $fn = 64);

        // Cable hole
        translate([inner_size[0] / 2 - cable_size[0], -cable_size[1] / 2, -$fudge]) cube([cable_size[0], cable_size[1], wth + 2 * $fudge]);
    }

    // Signature
    linear_extrude(wth + .4) {
        translate([0, inner_size[1] * +.3]) text(text = vsig[0], font = "Arial:bold", size = 4, halign = "center", valign = "center");
        text(text = vsig[1], font = "Arial:bold", size = 4, halign = "center", valign = "center");
        translate([0, inner_size[1] * -.3])text(text = vsig[2], font = "Arial:bold", size = 4, halign = "center", valign = "center");
    }
}

module bottom(hollow = false) {
    outer_size = [
        board_size[0] + board_clearance + 4 * wtv,
        board_size[1] + board_clearance + 4 * wtv,
        bottom_height - bottom_lip + wth
    ];
    inner_size = [
        board_size[0] + board_clearance,
        board_size[1] + board_clearance,
        bottom_height + wth + 2 * $fudge
    ];
    inner_radius = outer_radius - 2 * wtv;
    inner_lip_size = [
        board_size[0] + board_clearance + 2 * wtv,
        board_size[1] + board_clearance + 2 * wtv,
        bottom_height + wth
    ];
    inner_lip_radius = outer_radius - wtv;

    // Main shape
    difference() {
        union() {
            // Outer shape
            translate([-outer_size[0] / 2, -outer_size[1] / 2]) rcube(outer_size, outer_radius, $fn = 64);
            // Lip
            translate([-inner_lip_size[0] / 2, -inner_lip_size[1] / 2]) rcube(inner_lip_size, inner_lip_radius, $fn = 64);
        }

        // Main hole
        if(hollow) {
            translate([-inner_size[0] / 2, -inner_size[1] / 2, -$fudge]) rcube(inner_size, inner_radius, $fn = 64);
        } else {
            translate([-inner_size[0] / 2, -inner_size[1] / 2, wth]) rcube(inner_size, inner_radius, $fn = 64);
        }

    }

    // Signature
    linear_extrude(wth + .4) {
        translate([0, inner_size[1] * +.3]) text(text = vsig[0], font = "Arial:bold", size = 4, halign = "center", valign = "center");
        text(text = vsig[1], font = "Arial:bold", size = 4, halign = "center", valign = "center");
        translate([0, inner_size[1] * -.3])text(text = vsig[2], font = "Arial:bold", size = 4, halign = "center", valign = "center");
    }
}

module rcube(size, radius) {
    if(!is_list(radius)) {
        // The same radius on all corners
        rcube(size, [radius, radius, radius, radius]);
    } else if(len(radius) == 2) {
        // Different radii on top and bottom
        rcube(size, [radius[0], radius[0], radius[1], radius[1]]);
    } else if(len(radius) == 4) {
        // Different radius on different corners
        hull() {
            // BL
            if(radius[0] == 0) cube([1, 1, size[2]]);
            else translate([radius[0], radius[0]]) cylinder(r = radius[0], h = size[2]);
            // BR
            if(radius[1] == 0) translate([size[0] - 1, 0]) cube([1, 1, size[2]]);
            else translate([size[0] - radius[1], radius[1]]) cylinder(r = radius[1], h = size[2]);
            // TR
            if(radius[2] == 0) translate([size[0] - 1, size[1] - 1])cube([1, 1, size[2]]);
            else translate([size[0] - radius[2], size[1] - radius[2]]) cylinder(r = radius[2], h = size[2]);
            // TL
            if(radius[3] == 0) translate([0, size[1] - 1]) cube([1, 1, size[2]]);
            else translate([radius[3], size[1] - radius[3]]) cylinder(r = radius[3], h = size[2]);
        }
    } else {
        echo("ERROR: Incorrect length of 'radius' parameter. Expecting integer or vector with length 2 or 4.");
    }
}