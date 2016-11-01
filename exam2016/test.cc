// dist sum
std::vector<double>distances(num_particles);
std::vector<double>angles(num_particles);
double landmark_y = 0;
double landmark_x = landmark_id ? 0 : 300;
double disti;
double anglei;
double distsum = 0;
double anglesum = 0;
for (int i = 0; i < num_particles; i++) {
  disti = sqrt(pow(landmark_x - particles[i].x, 2) +
               pow(landmark_y - particles[i].y, 2));
  distances.push_back(disti);
  distsum += disti;
  anglei = acos((landmark_x - particles[i].x)/disti) - particles[i].theta;
  anglesum += anglei;
  angles.push_back(anglei);
            
 }
double distmean = distsum/num_particles;
double anglemean = anglesum/num_particles;

double distvariance = 0;
double anglevariance = 0;
for (int i = 0 ; i < num_particles; i++) {
  distvariance += pow(distmean - distances[i], 2);
  anglevariance += pow(anglemean - angles[i], 2);
 }
double stdvardist = sqrt(distvariance);
double stdvarangle = sqrt(distvariance);

void resample(std::vector<particle> *par) {
  int num_particles = par->size();
  std::vector<double> weightSumGraph;
  weightSumGraph.reserve(num_particles);
  for(int i = 0; i < num_particles; i++) {
    weightSumGraph.push_back(weightSumGraph.back() + par->at(i).weight);
  }
  std::vector<particle> pickedParticles; //(num_particles);
  double z;
  for (int i = 0; i < num_particles; i++) {
    z = randf();
    for (int t = 0; t < num_particles; t++) {
      if (z < weightSumGraph[t]) {
        continue;
      }
      pickedParticles.push_back(par->at(t));
      break;
    }
  }
  *par = pickedParticles;
  // removes all weights from vector at resets length 0.
  weightSumGraph.clear();
}
