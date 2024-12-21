#include "mavlink_service.hpp"
#include "comm/tcp_connector.hpp"
#include "comm/udp_connector.hpp"
#include "mavlink_comm_tcp.hpp"
#include "mavlink_comm_udp.hpp"
#include "mavlink_encoder.hpp"

#include "hako_mavlink_msgs/pdu_ctype_conv_mavlink_HakoHilSensor.hpp"
#include "hako_mavlink_msgs/pdu_ctype_conv_mavlink_HakoHilGps.hpp"
#include "hako_mavlink_msgs/pdu_ctype_conv_mavlink_HakoHilStateQuaternion.hpp"
#include "hako_mavlink_msgs/pdu_ctype_conv_mavlink_HakoHilActuatorControls.hpp"

using namespace hako::comm;
using namespace hako::mavlink;

MavLinkService::MavLinkService(MavlinkServiceIoType io_type, const IcommEndpointType *server_endpoint, const IcommEndpointType *client_endpoint)
{
    is_service_started_ = false;
    if (server_endpoint == nullptr)
    {
        std::cerr << "Invalid endpoint" << std::endl;
        return;
    }
    if (client_endpoint != nullptr)
    {
        client_endpoint_ = std::make_unique<IcommEndpointType>();
    }
    else {
        client_endpoint_ = nullptr;
    }
    server_endpoint_ = *server_endpoint;
    io_type_ = io_type;

    switch (io_type)
    {
    case MAVLINK_SERVICE_IO_TYPE_TCP:
        hako::comm::comm_init();
        comm_server_ = std::make_unique<TcpServer>();
        mavlink_comm_ = std::make_unique<MavLinkCommTcp>();
        break;
    case MAVLINK_SERVICE_IO_TYPE_UDP:
        comm_server_ = std::make_unique<UdpServer>();
        mavlink_comm_ = std::make_unique<MavLinkCommUdp>();
        break;
    default:
        std::cerr << "Invalid IO type" << std::endl;
        break;
    }
}
bool MavLinkService::sendMessage(MavlinkHakoMessage& message)
{
    if (!is_service_started_)
    {
        std::cerr << "Service is not started" << std::endl;
        return false;
    }
    if (comm_io_ == nullptr)
    {
        std::cerr << "Invalid comm io" << std::endl;
        return false;
    }
    MavlinkDecodedMessage decoded_message;
    switch (message.type) {
        case MAVLINK_MSG_TYPE_HIL_SENSOR:
        {
            hako_convert_pdu2mavlink_HakoHilSensor(message.data.hil_sensor, decoded_message.data.hil_sensor);
            break;
        }
        case MAVLINK_MSG_TYPE_HIL_ACTUATOR_CONTROLS:
        {
            hako_convert_pdu2mavlink_HakoHilActuatorControls(message.data.hil_actuator_controls, decoded_message.data.hil_actuator_controls);
            break;
        }
        case MAVLINK_MSG_TYPE_HIL_STATE_QUATERNION:
        {
            hako_convert_pdu2mavlink_HakoHilStateQuaternion(message.data.hil_state_quaternion, decoded_message.data.hil_state_quaternion);
            break;
        }
        case MAVLINK_MSG_TYPE_HIL_GPS:
        {
            hako_convert_pdu2mavlink_HakoHilGps(message.data.hil_gps, decoded_message.data.hil_gps);
            break;
        }
        default:
            std::cerr << "Invalid message type" << std::endl;
            return false;
    }
    return sendMessage(decoded_message);
}
bool MavLinkService::sendMessage(MavlinkDecodedMessage& message)
{
    if (comm_io_ == nullptr)
    {
        std::cerr << "Invalid comm io" << std::endl;
        return false;
    }
    mavlink_message_t mavlinkMsg;
    if (mavlink_encode_message(&mavlinkMsg, &message)) 
    {
        int sentDataLen = 0;
        char packet[MAVLINK_MAX_PACKET_LEN];
        int packetLen = mavlink_get_packet(packet, sizeof(packet), &mavlinkMsg);
        if (packetLen > 0) 
        {
            if (comm_io_->send(packet, packetLen, &sentDataLen)) 
            {
                //std::cout << "Sent MAVLink message with length: " << sentDataLen << std::endl;
            }
            else 
            {
                std::cerr << "Failed to send MAVLink message" << std::endl;
                return false;
            }
        }
        else {
            std::cerr << "Failed to get packet" << std::endl;
            return false;
        }
        return true;
    }
    else {
        std::cerr << "Failed to encode message" << std::endl;
        return false;
    }
}
bool MavLinkService::readMessage(MavlinkHakoMessage& message)
{
    (void)message;
    //TODO
    return true;
}
bool MavLinkService::start_Service()
{
    //TODO
    is_service_started_ = true;
    return is_service_started_;
}
void MavLinkService::stopService()
{
    //TODO
    is_service_started_ = false;
}