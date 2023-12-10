#ifndef _SENSOR_NOISE_HPP_
#define _SENSOR_NOISE_HPP_

#include "isensor_noise.hpp"
#include <random>

namespace hako::assets::drone {

class SensorNoise : public hako::assets::drone::ISensorNoise {
private:
    std::default_random_engine generator;
    std::normal_distribution<double> distribution;
    SensorNoise() {}
public:
    SensorNoise(double v) : distribution(0, v) {}
    SensorNoise(double m, double v) : distribution(m, v) {}    
    virtual ~SensorNoise() {}
    double add_noise(double data) override
    {
        double noise = distribution(generator);
        return data + noise;
    }
};

}

#endif /* _SENSOR_NOISE_HPP_ */