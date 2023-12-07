#ifndef _DRON_DYNAMICS_GROUND_FRAME_HPP_
#define _DRON_DYNAMICS_GROUND_FRAME_HPP_

#include "idrone_dynamics.hpp"

namespace hako::assets::drone {


class DroneDynamicsGroundFrame : public hako::assets::drone::IDroneDynamics {
private:
    /*
     * parameters
     */
    double param_mass;
    double param_drag;
    /*
     * internal state
     */
    DronePositionType position;           // Initialized to zero by default (glm::dvec3)
    DroneVelocityType velocity;           // Initialized to zero by default (glm::dvec3)
    DroneAngleType angle;                 // Initialized to zero by default (glm::dvec3)
    DroneAngularVelocityType angularVelocity; // Initialized to zero by default (glm::dvec3)

    DronePositionType next_position;           // Initialized to zero by default (glm::dvec3)
    DroneVelocityType next_velocity;           // Initialized to zero by default (glm::dvec3)
    DroneAngleType next_angle;                 // Initialized to zero by default (glm::dvec3)
    DroneAngularVelocityType next_angularVelocity; // Initialized to zero by default (glm::dvec3)

    DroneVelocityBodyFrameType velocityBodyFrame;
    DroneAngularVelocityBodyFrameType angularVelocityBodyFrame;

    double delta_time_sec;
    double total_time_sec;

    void run_x(const DroneThrustType &thrust, const DroneTorqueType &torque);
    void run_y(const DroneThrustType &thrust, const DroneTorqueType &torque);
    void run_z(const DroneThrustType &thrust, const DroneTorqueType &torque);
    void run_rx(const DroneThrustType &thrust, const DroneTorqueType &torque);
    void run_ry(const DroneThrustType &thrust, const DroneTorqueType &torque);
    void run_rz(const DroneThrustType &thrust, const DroneTorqueType &torque);

public:
    // Constructor with zero initialization
    DroneDynamicsGroundFrame(double dt)
    {
        this->total_time_sec = 0;
        this->delta_time_sec = dt;
        this->param_mass = 1;
        this->param_drag = 0;
    }
    virtual ~DroneDynamicsGroundFrame() {}

    void set_mass(double mass)
    {
        this->param_mass = mass;
    }
    void set_drag(double drag) override
    {
        this->param_drag = drag;
    }
    // Setters
    void set_pos(const DronePositionType &pos) override {
        position = pos;
    }

    void set_vel(const DroneVelocityType &vel) override {
        velocity = vel;
    }

    void set_angle(const DroneAngleType &ang) override {
        angle = ang;
    }

    void set_angular_vel(const DroneAngularVelocityType &angularVel) override {
        angularVelocity = angularVel;
    }

    // Getters
    DronePositionType get_pos() const override {
        return position;
    }

    DroneVelocityType get_vel() const override {
        return velocity;
    }

    DroneAngleType get_angle() const override {
        return angle;
    }

    DroneAngularVelocityType get_angular_vel() const override {
        return angularVelocity;
    }
    DroneVelocityBodyFrameType get_vel_body_frame() const override {
        return velocityBodyFrame;
    }
    DroneAngularVelocityBodyFrameType get_angular_vel_body_frame() const override {
        return angularVelocityBodyFrame;
    }

    // Implementation for the run function is required
    void run(const DroneThrustType &thrust, const DroneTorqueType &torque) override 
    {
        this->cache = drone_phys_calc_cache(this->angle);
        run_x(thrust, torque);
        run_y(thrust, torque);
        run_z(thrust, torque);
        run_rx(thrust, torque);
        run_ry(thrust, torque);
        run_rz(thrust, torque);

        //boundary condition
        if (this->next_position.data.z > 0) {
            this->next_position.data.z = 0;
            this->next_velocity.data.z = 0;
        }

        this->position = this->next_position;
        this->velocity = this->next_velocity;
        this->angle = this->next_angle;
        this->angularVelocity = this->next_angularVelocity;
        
        this->total_time_sec += this->delta_time_sec;
    }
};

}


#endif /* _DRON_DYNAMICS_GROUND_FRAME_HPP_ */