#include <cmath>
#include "robot.h"
#include "particles.h"
#include "misc.h"

float ir_correction(int index, IrProxy &ir) {
  return ir.GetRange(index) * 0.93569 - 0.103488;
}

bool check_sensor(int index, float threshold, IrProxy &ir) {
  return (ir_correction(index, ir) < threshold);
}

bool check_sensors(PlayerClient &robot, Position2dProxy &pp, IrProxy &ir, double threshold) {
  robot.Read();
  for (int i = 2; i < 9; i++) {
    if (check_sensor(i, threshold, ir)) {
      pp.SetSpeed(0, 0);
      return true;
    }
  }
  return false;
}

double check_right(IrProxy &ir) {
  return ir_correction(3, ir) + ir_correction(5, ir) + ir_correction(6, ir);
}

double check_left(IrProxy &ir) {
  return ir_correction(4, ir) + ir_correction(7, ir) + ir_correction(8, ir);
}

void avoid_obstacle(PlayerClient &robot, Position2dProxy &pp, IrProxy &ir, pos_t &pos) {
  robot.Read();
  double threshold = 1.5;
  double angle_turned = 0;
  double right = check_right(ir);
  double left = check_left(ir);
  while(right < threshold || left < threshold) {
    if (right > left) {
      turn(pp, pos, degrees_to_radians(5));
      angle_turned += 5;
    } else {
      turn(pp, pos, degrees_to_radians(-5));
      angle_turned -= 5;
    }
    right = check_right(ir);
    left = check_left(ir);
    robot.Read();
  }
  drive(robot, pp, ir, pos, 40, false);
  turn(pp, pos, -angle_turned);
}

void turn(Position2dProxy &pp, pos_t &pos, double turn_rad) {
  const double turn_speed = degrees_to_radians(20);
  pp.SetSpeed(0.0, turn_speed * ((turn_rad >= 0) ? 1 : -1));
  sleep(fabs(turn_rad / turn_speed));
  //sleep(0.3);
  pp.SetSpeed(0.0, 0.0);
  pos.turn += turn_rad;
}

void drive(PlayerClient &robot, Position2dProxy &pp, IrProxy &ir,
           pos_t &pos, double dist_cm, bool avoid_obstacles) {
  const double speed_cm = 10.0;
  const size_t n_time_slots = 10;
  const double ir_threshold = 0.5;

  double sleep_dur = fabs(dist_cm / speed_cm);
  double sleep_slot_dur = sleep_dur / n_time_slots;

  double time_start = current_time();

  bool found_obstacle = false;
  pp.SetSpeed(speed_cm / 100.0, 0.0);
  for (size_t i = 0; i < n_time_slots; i++) {
    if (avoid_obstacles && check_sensors(robot, pp, ir, ir_threshold)) {
      found_obstacle = true;
      break;
    }
    double time_target = time_start + (i + 1) * sleep_slot_dur;
    sleep(time_target - current_time());
  }
  double time_diff = current_time() - time_start;
  pp.SetSpeed(0.0, 0.0);
  double dur_ratio = time_diff / sleep_dur;
  double dist_cm_actual = dist_cm * dur_ratio;
  pos.x = pos.x + dist_cm_actual * cos(pos.turn);
  pos.y = pos.y + dist_cm_actual * sin(pos.turn);

  if (found_obstacle) {
    avoid_obstacle(robot, pp, ir, pos);
  }
}

bool handle_turning(Position2dProxy &pp, pos_t &pos, double &angle_var, double turn_rad) {
  if (angle_var < 0) {
    turn(pp, pos, -turn_rad);
    angle_var += radians_to_degrees(turn_rad);
    } else {
    turn(pp, pos, turn_rad);
    angle_var -= radians_to_degrees(turn_rad);
  }
  return fabs(angle_var) < radians_to_degrees(turn_rad);
  // if (abs(angle_var) < radians_to_degrees(turn_rad)) {
  //   angle_var = 0;
  //   return true;
  //   } else {
  //   return false;
  // }
}
