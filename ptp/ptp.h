#pragma once
#include <optional>
#include <vector>
#include <cstdint>

#include "transport.h"
#include "utils.h"

// ISO 15740 core opcodes (subset that’s broadly useful)
enum : std::uint16_t {
	PTP_OP_GetDeviceInfo        = 0x1001,
	PTP_OP_OpenSession          = 0x1002,
	PTP_OP_CloseSession         = 0x1003,
	PTP_OP_GetStorageIDs        = 0x1004,
	PTP_OP_GetStorageInfo       = 0x1005,
	PTP_OP_GetNumObjects        = 0x1006,
	PTP_OP_GetObjectHandles     = 0x1007,
	PTP_OP_GetObjectInfo        = 0x1008,
	PTP_OP_GetObject            = 0x1009,
	PTP_OP_GetThumb             = 0x100A,
	PTP_OP_DeleteObject         = 0x100B,
	PTP_OP_SendObjectInfo       = 0x100C,
	PTP_OP_SendObject           = 0x100D,
	PTP_OP_InitiateCapture      = 0x100E,
	PTP_OP_ResetDevice          = 0x1010,
	PTP_OP_PowerDown            = 0x1013,
	PTP_OP_GetDevicePropDesc    = 0x1014,
	PTP_OP_GetDevicePropValue   = 0x1015,
	PTP_OP_SetDevicePropValue   = 0x1016,
	PTP_OP_ResetDevicePropValue = 0x1017,
	PTP_OP_TerminateOpenCapture = 0x1018,
	PTP_OP_MoveObject           = 0x1019,
	PTP_OP_CopyObject           = 0x101A,
	PTP_OP_GetPartialObject     = 0x101B,
	PTP_OP_InitiateOpenCapture  = 0x101C,
};

enum : uint16_t {
	PTP_CONTAINER_COMMAND  = 1,
	PTP_CONTAINER_DATA     = 2,
	PTP_CONTAINER_RESPONSE = 3,
	PTP_CONTAINER_EVENT    = 4,
};

enum : uint16_t {
	PTP_OP_GET_DEVICE_INFO    = 0x1001,
	PTP_OP_OPEN_SESSION       = 0x1002,
	PTP_OP_CLOSE_SESSION      = 0x1003,
	PTP_OP_GET_OBJECT_HANDLES = 0x1007,
	PTP_OP_GET_OBJECT         = 0x1009,
	PTP_OP_INITIATE_CAPTURE   = 0x100E,
	PTP_OP_GET_DEVICE_PROP_VAL= 0x1015,
	PTP_OP_SET_DEVICE_PROP_VAL= 0x1016,
};

enum : std::uint16_t {
    PTP_EVENT_ObjectAdded = 0x4002
};

#pragma pack(push,1)
struct PtpContainerHeader {
	uint32_t total_length_bytes;      // includes header
	uint16_t container_type;          // 1/2/3/4
	uint16_t operation_or_response;   // opcode or response code
	uint32_t transaction_id;
};
#pragma pack(pop)

class CameraPTP {
    public:
        virtual ~CameraPTP() = default;

        // session
        virtual void open_session(std::uint32_t session_id=1);
        virtual void close_session();

        struct Response {
            std::uint16_t response_code{0};
            std::vector<std::uint32_t> params;
            std::vector<std::uint8_t>  data;
        };

        // core transaction
        virtual Response transact(std::uint16_t opcode,
                                    const std::vector<std::uint32_t>& params = {},
                                    const std::vector<std::uint8_t>* data_out = nullptr,
                                    bool expect_data_in = false);

        std::optional<uint32_t> wait_object_added(int timeout_ms, int poll_ms);

        // convenience (raw datasets; you can parse later)
        virtual std::vector<std::uint8_t>  get_device_info();
        virtual std::vector<std::uint32_t> get_storage_ids();
        virtual std::vector<std::uint8_t>  get_storage_info(std::uint32_t storage_id);
        virtual std::uint32_t              get_num_objects(std::uint32_t storage=0, std::uint32_t format=0, std::uint32_t assoc=0xFFFFFFFF);
        virtual std::vector<std::uint32_t> get_object_handles();
        virtual std::vector<std::uint32_t> get_object_handles(std::uint32_t storage, std::uint32_t format=0, std::uint32_t assoc=0xFFFFFFFF);
        virtual std::vector<std::uint8_t>  get_object_info(std::uint32_t handle);
        virtual std::vector<std::uint8_t>  get_object(std::uint32_t handle);
        virtual std::vector<std::uint8_t>  get_partial_object(std::uint32_t handle, std::uint32_t offset, std::uint32_t max_bytes);
        virtual std::vector<std::uint8_t>  get_thumb(std::uint32_t handle);
        virtual void                       send_object_info(const std::vector<std::uint8_t>& info_dataset);
        virtual void                       send_object(const std::vector<std::uint8_t>& object_bytes);
        virtual void                       delete_object(std::uint32_t handle);
        virtual void                       move_object(std::uint32_t handle, std::uint32_t storage, std::uint32_t parent);
        virtual void                       copy_object(std::uint32_t handle, std::uint32_t storage, std::uint32_t parent);
        virtual void                       initiate_capture(std::uint32_t storage=0, std::uint32_t format=0);
        virtual void                       initiate_open_capture();
        virtual void                       terminate_open_capture();
        virtual void                       reset_device();
        virtual void                       power_down();
        virtual std::vector<std::uint8_t>  get_device_prop_desc(std::uint16_t prop_code);
        virtual std::vector<std::uint8_t>  get_device_prop_value(std::uint16_t prop_code);
        virtual void                       set_device_prop_value(std::uint16_t prop_code, const std::vector<std::uint8_t>& raw);
        virtual void                       reset_device_prop_value(std::uint16_t prop_code);

        // transport-level helpers to mirror PTPy
        virtual std::vector<std::uint8_t> mesg(std::uint16_t opcode,
                                                const std::vector<std::uint32_t>& params={});
        virtual std::vector<std::uint8_t> event(unsigned timeout_ms=50);

    protected:
        explicit CameraPTP(Transport& t) : transport_(t) {}
        std::vector<std::uint8_t> read_full_container_();

        Transport& transport_;
        std::uint32_t next_tid_{1};
};