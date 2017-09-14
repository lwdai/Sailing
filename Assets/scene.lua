-- scene.lua

-- root --
rootnode = gr.node('root')


red = gr.material({1.0, 0.0, 0.0}, {0.1, 0.1, 0.1}, 30)
blue = gr.material({0.0, 0.0, 1.0}, {0.1, 0.1, 0.5}, 30)
green = gr.material({0.0, 1.0, 0.0}, {0.1, 0.1, 0.1}, 5)
white = gr.material({1.0, 1.0, 1.0}, {0.1, 0.1, 0.1}, 10)

-- land

land = gr.mesh('terrain', 'terrain')
rootnode:add_child(land)
land:set_material(green)

-- joint between land & ship
land_ship = gr.joint('land_ship_joint', {0.0, 0.0, 0.0}, {-360.0, 0.0, 360.0})
land_ship:translate(1.0,0.0,1.0)
land:add_child( land_ship )

-- ship

ship = gr.mesh('ship', 'ship')
ship:set_material( red )
land_ship:add_child(ship)
ship:scale(0.1,0.1,0.1)
ship:rotate('y', -90)

-- water

water = gr.mesh('water','water')
water:set_material(blue)
rootnode:add_child(water)


return rootnode

