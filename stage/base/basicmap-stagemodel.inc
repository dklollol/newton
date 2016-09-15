# World map properties
define basicmap model
(
  # Chose black to mimic e.g. walls.
  color "black"

  # If 1, this model can be gripped by a gripper and can be pushed around by
  # collisions with anything that has a non-zero obstacle_return.  We do not
  # want this to apply to a map object.
  gripper_return 0
  
  # Draw a nose on the model so we can see which way it points.  This is not
  # used as we use this object as a map.
  gui_nose 0

  # If 1, draw a bounding box around the model, indicating its size.  Most maps
  # will need a bounding box.  Using a bounding box will make the map seem
  # limited even though the world size is bigger than the map.
  boundary 1

  # If 1, draw a scaling grid over the model.  This is the grid you see in the
  # Stage window.  The grid is snapped to the map model and is useful as a
  # background.
  gui_grid 1
  
  # Define how the model can be moved by the mouse in the GUI window.  You
  # should not be able to move the "map".
  gui_movemask 0
  
  # This is non-documented property but it is used in some of the "official"
  # Stage examples when making map objects.
  gui_outline 0
)
