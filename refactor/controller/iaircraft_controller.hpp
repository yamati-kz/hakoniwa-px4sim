#ifndef _IAIRCRAFT_CONTROLLER_HPP_
#define _IAIRCRAFT_CONTROLLER_HPP_

#include "controller/aircraft_controller_types.h"
#include "physics/drone_physics.hpp"

namespace hako::controller {
class IAircraftController {
public:
    virtual ~IAircraftController() {}
    virtual void reset() = 0;
    virtual mi_aircraft_control_out_t run(mi_aircraft_control_in_t& in) = 0;
};
}


#endif /* _IAIRCRAFT_CONTROLLER_HPP_ */