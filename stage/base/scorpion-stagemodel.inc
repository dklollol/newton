# The Evolution Robotics Scorpion Robot has IR sensors, a digital sensor and a
# bumper device.  The robot itself is described below, but its devices (sensors
# and bumper) are described in other files included here below.  The creation of
# the devices on the robot is done last in the robot description.  The sensor
# and bumper are models as well as the robot itself.


# A model of the Scorpion robots infrared sensor defined as a ranger model
define scorpionirrangesensors ranger
(
  # There is 13 Sharp GP2D12 IR sensor on the Scorpion
  scount 13

  # Define the position of each sensor:
  # spose[index] [xpos ypos heading]
  # The position is relative to its parents - that is the robots coordinate
  #  system because this model is intended to be included in the robot model
  # The Scorpion robot documentation also state placement of sensor in
  #  the robot coordinate system.
  
  # Index matches the defintion in the Player driver for the robot
  
  # Scorpion Device ID: IR_bn_ene
  # Position (cm): x=12.6 y=9,2 z=7.1 cm
  # Angles (rad): Roll=3.142, Pitch=0, Yaw=-1.134
  spose[0] [ 0.126 0.092 -64.973 ] 
  # Scorpion Device ID: IR_bn_wnw
  # Position (cm): x=12.6 y=-9,2 z=7.1 cm
  # Angles (rad): Roll=0, Pitch=0, Yaw=1.134
  spose[1] [ 0.126 -0.092 64.973 ]
  # Scorpion Device ID: IR_bn_n
  # Position (cm): x=4.9 y=0 z=7.2 cm
  # Angles (rad): Roll=0, Pitch=0, Yaw=0
  spose[2] [ 0.049 0.000 0 ]
  # Scorpion Device ID: IR_bn_ne
  # Position (cm): x=9.2 y=6.4 z=7.2 cm
  # Angles (rad): Roll=3.142, Pitch=0, Yaw=-0.611
  spose[3] [ 0.092 0.064 -35.008 ] 
  # Scorpion Device ID: IR_bn_nw
  # Position (cm): x=9.2 y=-6.4 z=7.2 cm
  # Angles (rad): Roll=0, Pitch=0, Yaw=0.611
  spose[4] [ 0.092 -0.064 35.008 ] 
  # Scorpion Device ID: IR_te_nnw
  # Position (cm): x=1.6 y=-17.3 z=11.4 cm
  # Angles (rad): Roll=3.142, Pitch=0, Yaw=0.175
  spose[5] [ 0.016 -0.173 10.027 ] 
  # Scorpion Device ID: IR_te_nne
  # Position (cm): x=-6.5 y=-16.9 z=14.6 cm
  # Angles (rad): Roll=1.571, Pitch=0, Yaw=-0.262
  spose[6] [ -0.065 -0.169 -15.011 ] 
  # Scorpion Device ID: IR_tw_nnw
  # Position (cm): x=-6.5 y=16.9 z=14.6 cm
  # Angles (rad): Roll=-1.571, Pitch=0, Yaw=0.262
  spose[7] [ -0.065 0.169 15.011 ] 
  # Scorpion Device ID: IR_tw_nne
  # Position (cm): x=1.6 y=17.3 z=11.4 cm
  # Angles (rad): Roll=0, Pitch=0, Yaw=0.-175
  spose[8] [ 0.016 0.173 -10.027 ] 
  # Scorpion Device ID: IR_bs_w
  # Position (cm): x=-17.2 y=1.9 z=8.7 cm
  # Angles (rad): Roll=3.142, Pitch=0, Yaw=1.571
  spose[9] [ -0.172 0.019 90.011 ] 
  # Scorpion Device ID: IR_bs_e
  # Position (cm): x=-17.2 y=-1.9 z=8.7 cm
  # Angles (rad): Roll=3.142, Pitch=0, Yaw=-1.571
  spose[10] [ -0.172 -0.019 -90.011 ] 
  # Scorpion Device ID: IR_bw_s
  # Position (cm): x=-7.7 y=6.9 z=8.7 cm
  # Angles (rad): Roll=3.142, Pitch=0, Yaw=3.054
  spose[11] [ -0.077 0.069 174.981 ] 
  # Scorpion Device ID: IR_be_s
  # Position (cm): x=-7.7 y=-6.9 z=8.7 cm
  # Angles (rad): Roll=3.142, Pitch=0, Yaw=-3.054
  spose[12] [ -0.077 -0.069 -174.981 ] 

  
  # Define the field of view of each transducer:
  # sview [range_min range_max view_angle]
  # But instead of doing it for every sensor, we do it for all by omitting
  #  the index as in: ssize[3] [0.01 0.05]
  # The Sharp GP2D12 sensor detects distance from 10 to 80 cm (according to 
  #  Sharp documentation)
  # View angle is unsure, but some says only 5 degree
  sview [0.10 .80 5]

  
  # Define the size of each sensor [xsize ysize] in meters
  # If all have same size this could be done with eg.: ssize [0.01 0.05]
  # The Sharp GP2D12 sensor dimension is (according to Sharp documentation):
  # (looking into the lens)
  # Width: 	40.2 mm (37 mm + 0.5*2*3.2mm)
  # Depth: 	13.5 mm
  # Height: 	13.0 mm
  ssize [0.013 0.040]
  
  # Not all sensors have the same size as defined above.
  # It would be more correct to overwrite these to sensors sizes with the
  #  correct size- but it won't work with Stage though is should according
  #  to the documentation.
   
  # These sensors is "standing", so we set different sizes
 #ssize[6] [ 0.013 0.0135 ]
 #ssize[7] [ 0.013 0.0135 ]
  
)



# The model of the Scorpion robot defined from the position model
define scorpionrobot position
(
  # - Properties of the "position" model -
  # The Scorpion have differential steering (a seperate motor for each wheel)
  drive "diff"

  # - Robots localization -
  # If "gps" the position model reports its position with perfect accuracy. 
  # If "odom", a simple odometry model is used and position data drifts from
  #  the ground truth over time. The odometry model is parameterized by the
  #  odom_error property. 
  # As standard use a precise odometry in Stage because we often prototype
  # It can be overwritten when the robot object is created
  localization "gps"


  # - Properties of general model ("model)" -
  # This specify the pose of the model in its parent's coordinate system
  # As ERSP world coordinate system and the robots coordinate system is
  #  coincident when turning the robot on we set this to origo
  # Typically the this start position is overwritten when object is created
  pose [0 0 0]
  # Size of the Scorpion robot in 2D projection on the floor is:
  # 43,9 cm (length) and 40,6 cm (width)
  size [0.44 0.41]
  # The Scorpion robot's center - coincident with origo the robot's 
  #  coordinate system as ERSP uses. It is -6 cm a the X-axis
  # Heading is not used
  origin [-0.06 0.0 0]
  # Velocity of the model is zero at start
  velocity [0 0 0]
  
  # Choosing from X11 database (rgb.txt) looks darker gray
  color "DarkGray"

  # The followin lines determine how the model appears in various sensors
  
  # Our robot should collide with other models that have this property set
  obstacle_return 1
  
  # Even though Scorpion robots don't have laser they can be seen by laser
  laser_return 1
  
  # ... it will also be detected by ranger sensors
  ranger_return 1
  
  # ... if other robots have a gripper this can "grap" the robot
  gripper_return 1

  # - GUI properties of the general model ("model") -
  # Draw a nose on the model so we can see where the bumper is placed
  gui_nose 1
  
  # Scaling grid over the robot is not used. Typically the map model has it
  gui_grid 0
  
  # Allow to move the model with a mouse in the GUI window
  gui_movemask 1
  
  
  # - Unit square body shape properties of the general model ("model") -
  # This polygon approximates the shape of the robot
  polygons 1
  polygon[0].points 4
  polygon[0].point[0] [  0.06  0.21 ]
  polygon[0].point[1] [  0.06 -0.21 ]
  polygon[0].point[2] [ -0.38 -0.21 ]
  polygon[0].point[3] [ -0.38  0.21 ]

  # - Devices on the robots -
  # Create robots sensors and bumper - they are models as well.
  # They are describe above (included from other files) but first created here
  scorpionirrangesensors()
  # Binary sensors not supported! See above
 #scorpionirbinarysensors()
  # Bumper not supported! See above
 #scorpionbumper()
)
