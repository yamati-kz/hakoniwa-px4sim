set(HAKO_CMAKE_VERSION ${HAKO_CMAKE_VERSION})

project(hakoniwa-px4sim
    LANGUAGES C CXX
)

set(OS_TYPE "posix")
if(WIN32)
    set(OS_TYPE "win")
endif(WIN32)
MESSAGE(STATUS "OS_TYPE=" ${OS_TYPE})
include(FetchContent)
FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

set(SOURCE_FILES
    comm/tcp_connector.cpp
    mavlink/mavlink_dump.cpp
    mavlink/mavlink_decoder.cpp
    mavlink/mavlink_encoder.cpp

    hako/pdu/hako_pdu_data.cpp
    ${HAKONIWA_SOURCE_DIR}/hako_capi.cpp
    hako/runner/hako_px4_master.cpp

    threads/px4sim_thread_receiver.cpp
    threads/px4sim_thread_sender.cpp

    ${PHYSICS_SOURCE_DIR}/body_physics.cpp
    ${PHYSICS_SOURCE_DIR}/rotor_physics.cpp
    ${PHYSICS_SOURCE_DIR}/drone_physics_c.cpp
    
    assets/drone/controller/drone_pid_control.cpp
    assets/drone/controller/sample_controller.cpp
    assets/drone/aircraft/aircraft_factory.cpp

    utils/hako_params.cpp
    utils/hako_module_loader.cpp
    modules/hako_phys.cpp
    modules/hako_sim.cpp
    modules/hako_pid.cpp
    modules/hako_ext.cpp
    modules/hako_replay.cpp
    px4sim_main.cpp
)
if(WIN32)
else()
    list(APPEND SOURCE_FILES
        comm/udp_connector.cpp
        mavlink/mavlink_capture.cpp
        mavlink/mavlink_capture_replay.cpp
        threads/px4sim_thread_replay.cpp
        threads/px4sim_thread_capture.cpp
        modules/hako_bypass.cpp
    )
endif()
add_executable(hako-px4sim ${SOURCE_FILES} "utils/hako_osdep.h")

target_include_directories(
    hako-px4sim
    PRIVATE /usr/local/include
    PRIVATE /mingw64/include
    PRIVATE ${MAVLINK_SOURCE_DIR}/all
    PRIVATE ${PROJECT_SOURCE_DIR}
    PRIVATE ${HAKONIWA_PDU_SOURCE_DIR}
    PRIVATE ${HAKONIWA_SOURCE_DIR}
    PRIVATE ${PROJECT_SOURCE_DIR}/assets/drone/include
    PRIVATE ${GLM_SOURCE_DIR}
    PRIVATE ${HAKONIWA_CORE_SOURCE_DIR}/include
    PRIVATE ${PHYSICS_SOURCE_DIR}
    PRIVATE ${CONTROL_SOURCE_DIR}/include
    PRIVATE ${CONTROL_SOURCE_DIR}/workspace
    PRIVATE ${CONTROL_SOURCE_DIR}/workspace/DroneFlightController
    PRIVATE ${CONTROL_SOURCE_DIR}/workspace/DroneFlightController/include
    PRIVATE ${CONTROL_SOURCE_DIR}/workspace/DroneFlightController/utils
    PRIVATE ${SENSOR_SOURCE_DIR}/include
    PRIVATE ${SENSOR_SOURCE_DIR}/sensors/gyro/include
    PRIVATE ${nlohmann_json_SOURCE_DIR}/single_include
    PRIVATE ${MATLAB_SOURCE_DIR}
)

target_link_libraries(
    hako-px4sim 
    hakoarun
    drone_physics_matlab
)


add_executable(
    px4sim_manual
    px4sim_manual.cpp
)

target_include_directories(
    px4sim_manual
    PRIVATE src/include
    PRIVATE /usr/local/include
    PRIVATE ${HAKONIWA_PDU_SOURCE_DIR}
    PRIVATE ${HAKONIWA_ASSET_DIR}
    PRIVATE ${PHYSICS_SOURCE_DIR}
)

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    target_link_libraries(
        px4sim_manual
        assets
    )
elseif(WIN32)
    target_link_libraries(
        px4sim_manual
        assets
    )
else()
    target_link_libraries(
        px4sim_manual
        assets
        atomic
    )
endif()
