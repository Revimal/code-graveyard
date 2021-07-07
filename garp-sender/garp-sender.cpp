#include <iostream>
#include <sstream>

#include <tins/tins.h>

class GARP : public Tins::ARP
{
private:
    Tins::ARP::hwaddress_type& sender_hw_;
public:
    GARP(Tins::ARP::ipaddress_type& garp_ip, Tins::ARP::hwaddress_type& sender_hw, bool is_reply = false) :
    Tins::ARP(garp_ip, garp_ip, Tins::ARP::hwaddress_type::broadcast, sender_hw), sender_hw_(sender_hw) {
        if (is_reply)
            opcode(Tins::ARP::REPLY);
        else
            opcode(Tins::ARP::REQUEST);
    }

    Tins::EthernetII build() {
        return (Tins::EthernetII(Tins::ARP::hwaddress_type::broadcast, sender_hw_) / *this);
    }
};

class GARPSender
{
public:
    typedef Tins::ARP::hwaddress_type MACAddress;
    enum class SendOperation { SEND_REQUEST, SEND_REPLY, SEND_BOTH };

    GARPSender(Tins::IPv4Address& target_ip, SendOperation send_operation = SendOperation::SEND_BOTH) :
    target_ip_(target_ip), sender_if_(target_ip), send_operation_(send_operation) {
        sender_ip_ = sender_if_.ipv4_address();
        sender_mac_ = sender_if_.hw_address();
    };

    void send() {
        GARP garp_pdu(sender_ip_, sender_mac_);
        Tins::EthernetII ether_pdu = garp_pdu.build();

        packet_sender_.send(ether_pdu, sender_if_);
    }

    Tins::IPv4Address target_ip() const {
        return target_ip_;
    }

    Tins::IPv4Address sender_ip() const {
        return sender_ip_;
    }

    MACAddress sender_mac() const {
        return sender_mac_;
    }

    SendOperation send_operation() const {
        return send_operation_;
    }

    friend std::ostream& operator<<(std::ostream& output, const GARPSender& sender) {
        Tins::IPv4Address& sender_ip = const_cast<Tins::IPv4Address&>(sender.sender_ip_);
        MACAddress& sender_mac = const_cast<MACAddress&>(sender.sender_mac_);
        SendOperation& send_operation = const_cast<SendOperation&>(sender.send_operation_);

        output << "Interface-info: " << sender.sender_if_.name() << std::endl;
        output << "Interface-link: " << (sender.sender_if_.is_up() ? "UP" : "DOWN") << std::endl;

        switch (send_operation) {
            case SendOperation::SEND_BOTH: /* fall-through */
            case SendOperation::SEND_REPLY:
            {
                output << dump_garp(GARP(sender_ip, sender_mac, true)) << std::endl;
                if (send_operation != SendOperation::SEND_BOTH)
                    break;
            } /* fall-through */
            case SendOperation::SEND_REQUEST:
                output << dump_garp(GARP(sender_ip, sender_mac, false)) << std::endl;
        }

        return output;
    }

private:
    Tins::PacketSender packet_sender_;
    Tins::IPv4Address target_ip_;
    Tins::IPv4Address sender_ip_;
    Tins::NetworkInterface sender_if_;
    MACAddress sender_mac_;
    SendOperation send_operation_;

    static std::string dump_garp(GARP&& garp_pdu) {
        std::stringstream output;

        output << "GARP-info: ";
        output << "Source(IP: " << garp_pdu.sender_ip_addr() << " / MAC: " << garp_pdu.sender_hw_addr() << ")";
        output << " / ";
        output << "Destination(IP: " << garp_pdu.target_ip_addr() << " / MAC: " << garp_pdu.target_hw_addr() << ")";
        output << " - " << (garp_pdu.opcode() == Tins::ARP::REQUEST ? "REQUEST" : "REPLY");

        return output.str();
    }
};

int main(int argc, char *argv[])
{
    Tins::IPv4Address target_ip;

    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <Target-IP>" << std::endl;
        return 1;
    }

    try {
        target_ip = argv[1];
    }
    catch (...) {
        std::cout << "Invalid Target-IP: " << argv[1] << std::endl;
        return 1;
    }

    GARPSender garp_sender(target_ip);

    std::cout << garp_sender;
    garp_sender.send();
    std::cout << "SENT" << std::endl;

    return 0;
}