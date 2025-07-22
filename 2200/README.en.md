![逐飞LOGO](https://images.gitee.com/uploads/images/2019/0924/114256_eaf16bad_1699060.png "逐飞科技logo 中.png")
# SEEKFREE MM32F327XG8P Opensource Library

#### Description
The MM32F327XG8P open source library is produced by FlyTech for participating in various competitions and using MM32F327XG8P for product development.

#### The advantages of the open source library
1.  **Driver functions that repackage common functionality** 
- In the official SDK on the basis of the secondary packaging, simple steps, more convenient to use a variety of internal and peripheral functions.
2.  **Driver functions that integrate common modules** 
- Various external peripheral module drivers have been integrated in the open source library, such as ICM20602 gyroscope driver, IPS LCD driver and total drilling camera driver. The peripheral module can be used directly by calling functions to save development time.

#### Work environment preparation
1.  **MM32F327XG8P Hardware Environment** 
- It is recommended to use our MM32F327XG8P core board.[Click here to buy](https://item.taobao.com/item.htm?spm=a1z10.5-c-s.w4002-22508770847.12.52ec1e6ceRhpgG&id=669592398259)
2.  **software development environment** 
（IAR or MDK can choose either）
- IAR Recommended version:IAR Embedded Workbench for ARM V8.32.4
- MDK Recommended version:MDK v5.33 and above.（After version 5.26, it added support for DAP emulator V2 version. You can use the WinUSB mode of our DAP emulator for faster download）
3.  **downloader & debugger** 
（DAP or J-Link can choose either）
- DAP:Recommended use of our DAP emulator, dual download mode, can achieve higher download speed in the supported environment.
- J-Link:Make sure your J-Link emulator hardware version is V9 or higher (J-Link OB is not supported). And the J-Link driver version is V6.40 or higher.

#### Instructions

1.  **Download the open source library** Click the clone/download button on the right side of the page to save the project file to the local. You can download it by using a Git Clone or by downloading a ZIP archive. It is recommended to use Git to clone the project directory locally so that you can use Git to keep up with your open source libraries at any time. For tutorials on how to use the cloud and Git, see the following link [https://gitee.com/help](at).
2.  **Open Project** Open the downloaded project folder (if downloaded as a ZIP file, please unzip the package). Before you open a project, make sure your IDE meets the requirements in the Environment Preparation section. Otherwise, there may be open project times error, prompt lost directory information and other problems.
- If you are using an IAR IDE, the Project files save in the Project/IAR folder.
- If you are using an MDK IDE, the Project files save in the Project/MDK folder.
