
#pragma once

namespace QUICGemPOC
{
    // System Component TypeIds
    inline constexpr const char* QUICGemPOCSystemComponentTypeId = "{D4F7B76C-B5E2-43D7-9765-DC8F2AC3D2DD}";
    inline constexpr const char* QUICGemPOCEditorSystemComponentTypeId = "{EB2E646E-2077-40B3-855E-FB31ECD9BBD0}";

    // Module derived classes TypeIds
    inline constexpr const char* QUICGemPOCModuleInterfaceTypeId = "{5428E79F-8F09-4A56-BD71-F6B6E9B73B03}";
    inline constexpr const char* QUICGemPOCModuleTypeId = "{FCBB0CC8-14D0-423B-A0D5-1BAB1CC6C9A0}";
    // The Editor Module by default is mutually exclusive with the Client Module
    // so they use the Same TypeId
    inline constexpr const char* QUICGemPOCEditorModuleTypeId = QUICGemPOCModuleTypeId;

    // Interface TypeIds
    inline constexpr const char* QUICGemPOCRequestsTypeId = "{04BA8057-7E79-4A40-8803-72DFB9DE35E1}";
} // namespace QUICGemPOC
