# YC16XX SDK

## 1.简介

​	YC16XX是一颗高集成、低功耗BLE SOC芯片，采用32bit RISC-V 内核处理器，支持单精度浮点运算，最高主频256MHz，支持内部RC或外部高速晶体时钟源，内部集成BUCK电路和锂电池充电管理模块，支持BLE5.4、single mesh协议栈和2.4G私有协议，具有UART、SPI、IIC、PWM、USB、SADC、MIC、DAC、CAN及NEC(IR)等丰富外设；支持固件加密。

## 2.SDK开发包说明

- YC16xx_SDK\DOC：芯片简介、datasheet、模块开发说明等文档

- YC16xx_SDK\Hardware：硬件设计注意事项、参考原理图及开发板硬件资料等文档

- YC16xx_SDK\SDK：软件开发包

  -  Libraries：库文件

  - ModuleDemo：各模块示例demo

  -  util：开发环境相关工具
     -  download.py：下载脚本
     -  gendat.py：固件整合脚本
     -  link_script.ld：链接脚本
     -  yc1161_eide_project_template.ept：工程模板
     -  tool：开发环境相关软件工具
        - e：Linux环境下的芯片调试工具
        - e.exe：Windows环境下的芯片调试工具
        - hid_ota_tool.exe：OTA工具
  

