#pragma once
#include <optional>
#include <vector>
#include <cstdint>

#include "ptp/transport.h"
#include "utils/utils.h"

// ISO 15740 core opcodes (subset that’s broadly useful)
enum : uint16_t {
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
    PTP_RESP_Undefined=0x2000,
    PTP_RESP_OK=0x2001,
    PTP_RESP_GeneralError=0x2002,
    PTP_RESP_SessionNotOpen=0x2003,
    PTP_RESP_InvalidTransactionID=0x2004,
    PTP_RESP_OperationNotSupported=0x2005,
    PTP_RESP_ParameterNotSupported=0x2006,
    PTP_RESP_IncompleteTransfer=0x2007,
    PTP_RESP_InvalidStorageId=0x2008,
    PTP_RESP_InvalidObjectHandle=0x2009,
    PTP_RESP_DevicePropNotSupported=0x200A,
    PTP_RESP_InvalidObjectFormatCode=0x200B,
    PTP_RESP_StoreFull=0x200C,
    PTP_RESP_ObjectWriteProtected=0x200D,
    PTP_RESP_StoreReadOnly=0x200E,
    PTP_RESP_AccessDenied=0x200F,
    PTP_RESP_NoThumbnailPresent=0x2010,
    PTP_RESP_SelfTestFailed=0x2011,
    PTP_RESP_PartialDeletion=0x2012,
    PTP_RESP_StoreNotAvailable=0x2013,
    PTP_RESP_SpecificationByFormatUnsupported=0x2014,
    PTP_RESP_NoValidObjectInfo=0x2015,
    PTP_RESP_InvalidCodeFormat=0x2016,
    PTP_RESP_UnknownVendorCode=0x2017,
    PTP_RESP_CaptureAlreadyTerminated=0x2018,
    PTP_RESP_DeviceBusy=0x2019,
    PTP_RESP_InvalidParentObject=0x201A,
    PTP_RESP_InvalidDevicePropFormat=0x201B,
    PTP_RESP_InvalidDevicePropValue=0x201C,
    PTP_RESP_InvalidParameter=0x201D,
    PTP_RESP_SessionAlreadyOpened=0x201E,
    PTP_RESP_TransactionCanceled=0x201F,
    PTP_RESP_SpecificationOfDestinationUnsupported=0x2020,
    PTP_RESP_InvalidEnumHandle=0x2021,
    PTP_RESP_NoStreamEnabled=0x2022,
    PTP_RESP_InvalidDataset=0x2023,
};

enum : uint16_t {
    PTP_EVENT_Undefined=0x4000,
    PTP_EVENT_CancelTransaction=0x4001,
    PTP_EVENT_ObjectAdded=0x4002,
    PTP_EVENT_ObjectRemoved=0x4003,
    PTP_EVENT_StoreAdded=0x4004,
    PTP_EVENT_StoreRemoved=0x4005,
    PTP_EVENT_DevicePropChanged=0x4006,
    PTP_EVENT_ObjectInfoChanged=0x4007,
    PTP_EVENT_DeviceInfoChanged=0x4008,
    PTP_EVENT_RequestObjectTransfer=0x4009,
    PTP_EVENT_StoreFull=0x400A,
    PTP_EVENT_DeviceReset=0x400B,
    PTP_EVENT_StorageInfoChanged=0x400C,
    PTP_EVENT_CaptureComplete=0x400D,
    PTP_EVENT_UnreportedStatus=0x400E,
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