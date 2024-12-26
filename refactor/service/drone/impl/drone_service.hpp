#ifndef _DRONE_SERVICE_HPP_
#define _DRONE_SERVICE_HPP_

#include "service/drone/idrone_service.hpp"
#include "service/drone/impl/idrone_service_operation.hpp"
#include "service/drone/impl/drone_service_rc.hpp"
#include "service/drone/impl/drone_service_api.hpp"

#include "aircraft/iaircraft_container.hpp"
#include "controller/iaircraft_mixer.hpp"
#include "controller/aircraft_controller_container.hpp"
#include <array>
#include <stdexcept>
#include <memory>

using namespace hako::aircraft;
using namespace hako::controller;

namespace hako::service::impl {


class DroneService : public IDroneService {
public:
    DroneService(std::shared_ptr<IAirCraft> aircraft, std::shared_ptr<IAircraftController> controller)
        : aircraft_(aircraft), controller_(controller) {
        simulation_time_usec_ = 0;
        delta_time_usec_ = 0;
        if (controller_->is_radio_control()) {
            drone_service_operation_ = std::make_unique<DroneServiceRC>();
        }
        else {
            drone_service_operation_ = std::make_unique<DroneServiceAPI>(aircraft_);
        }
        if (drone_service_operation_ == nullptr) {
            throw std::runtime_error("Failed to create drone service operation");
        }
        reset();
    }
    ~DroneService() override = default;

    bool startService(uint64_t deltaTimeUsec) override {
        delta_time_usec_ = deltaTimeUsec;
        simulation_time_usec_ = 0;
        is_service_available_ = true;
        return true;
    }

    void advanceTimeStep() override;

    void stopService() override {
        // Nothing to do
        is_service_available_ = false;
    }

    void resetService() override {
        controller_->reset();
        aircraft_.reset();
        reset();
        drone_service_operation_->reset();
    }
    bool isServiceAvailable() override {
        return is_service_available_;
    }
    uint64_t getSimulationTimeUsec() override {
        return simulation_time_usec_;
    }

    bool write_pdu(HakoniwaDronePduDataType& pdu) override {
        if (pdu.id >= HAKONIWA_DRONE_PDU_DATA_ID_TYPE_NUM) {
            throw std::out_of_range("write_pdu id out of range");
        }
        while (pdu_data_[pdu.id].is_busy.exchange(true)) {
            std::this_thread::yield();
        }
        pdu_data_[pdu.id].data.pdu = pdu.pdu;
        pdu_data_[pdu.id].is_dirty.store(true);
        pdu_data_[pdu.id].is_busy.store(false);
        return true;
    }

    bool read_pdu(HakoniwaDronePduDataType& pdu) override {
        if (pdu.id >= HAKONIWA_DRONE_PDU_DATA_ID_TYPE_NUM) {
            throw std::out_of_range("read_pdu id out of range");
        }
        while (pdu_data_[pdu.id].is_busy.exchange(true)) {
            std::this_thread::yield();
        }
        pdu.pdu = pdu_data_[pdu.id].data.pdu;
        pdu_data_[pdu.id].is_dirty.store(false);
        pdu_data_[pdu.id].is_busy.store(false);
        return true;
    }
    void peek_pdu(HakoniwaDronePduDataType& pdu) override {
        if (pdu.id >= HAKONIWA_DRONE_PDU_DATA_ID_TYPE_NUM) {
            throw std::out_of_range("peek_pdu id out of range");
        }
        while (pdu_data_[pdu.id].is_busy.exchange(true)) {
            std::this_thread::yield();
        }
        pdu.pdu = pdu_data_[pdu.id].data.pdu;
        pdu_data_[pdu.id].is_busy.store(false);
    }

private:
    uint64_t simulation_time_usec_ = 0;
    uint64_t delta_time_usec_ = 0;
    bool is_service_available_ = false;
    std::shared_ptr<IAirCraft> aircraft_;
    std::shared_ptr<IAircraftController> controller_;
    std::array<HakoniwaDronePduDataControlType, HAKONIWA_DRONE_PDU_DATA_ID_TYPE_NUM> pdu_data_ = {};
    AircraftInputType aircraft_inputs_ = {};
    mi_aircraft_control_in_t controller_inputs_ = {};
    mi_aircraft_control_out_t controller_outputs_ = {};
    PwmDuty pwm_duty_ = {};
    std::unique_ptr<IDroneServiceOperation> drone_service_operation_;

    void reset()
    {
        aircraft_inputs_ = {};
        controller_inputs_ = {};
        controller_outputs_ = {};
        pwm_duty_ = {};

        for (int i = 0; i < HAKONIWA_DRONE_PDU_DATA_ID_TYPE_NUM; i++) {
            pdu_data_[i].is_busy.store(false);
            pdu_data_[i].data.id = static_cast<HakoniwaDronePduDataIdType>(i);
            pdu_data_[i].data.pdu = {};
            pdu_data_[i].is_dirty.store(false);
        }
    }

    void setup_aircraft_inputs();
    void setup_controller_inputs();
    void write_back_pdu();
};

}

#endif /* _DRONE_SERVICE_HPP_ */
