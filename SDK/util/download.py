import os
import sys
from datetime import datetime

def e_cmd(cmd:str) -> str:
    with os.popen(cmd) as process:
        return process.read().strip()

def cpu_stop():
    stop_result = ""
    start_time = datetime.now()
    while True:
        result = e_cmd("e p")
        e_cmd("e 9f08 11")
        e_cmd("e 8050 1")
        current_time = datetime.now()
        time_diff = (current_time-start_time).total_seconds()
        print(f"\rstop cpu {int(time_diff)} ", end="", flush=True)
        if stop_result == result and result.find("Stopped") != -1:
            print(f'\r{result}')
            break
        else:
            stop_result = result
            e_cmd(f"e 8054 {result.split(',')[1].strip().split(' ')[1]}")
            if time_diff>60:   #秒
                print(f'\r{result}')
                print("stop cpu timeout")
                sys.exit(-1)

def main():
    if len(sys.argv)>=2:
        if not os.path.exists(sys.argv[1]):
            print(sys.argv[1],"not exist!")
            sys.exit(-1)

        if len(sys.argv)<3:
            ch = 0
        else:
            ch = int(sys.argv[2],16)
        os.environ['ch'] = f'{ch}'
        print("ch =", os.environ.get('ch'))
        #sync
        start_time = datetime.now()
        while True:
            result = e_cmd("e z")
            if result[-2:] != '02':
                current_time = datetime.now()
                if (current_time-start_time).total_seconds()>3:   #3秒超时
                    print(result)
                    print("sync fail")
                    sys.exit(-1)
            else:
                print(result)
                break

        #stop cpu & wdt
        # e_cmd("e k")
        cpu_stop()

        #set io
        io_config_file_path = os.path.dirname(os.path.abspath(sys.argv[0]))
        fast_download_fw_path = os.path.join(io_config_file_path, "fast_download.rom")
        io_config_file_path = os.path.join(io_config_file_path, "io_config.txt")
        io_config_cmd_list = []
        with open(io_config_file_path, 'r') as ioConfigFile:
            io_config_cmd_list = [line.strip() for line in ioConfigFile.readlines()]
        io_config_cmd = '&'.join(io_config_cmd_list)
        result = e_cmd(io_config_cmd)

        #擦除offset及固件头区域
        result = e_cmd("e fa")
        result = e_cmd("e fs 0")
        result = e_cmd("e fe 0 2000 2> null")
        #回读显示擦除结果
        result = e_cmd("e fr 1000 10 2> null")
        print(result.splitlines()[1])
        e_cmd("e k")
        #stop cpu & wdt
        cpu_stop()

        #快速指令编程
        result = e_cmd(f'e h "{fast_download_fw_path}" 0xc000 2> null')
        result = e_cmd("e r dpc 1000080")
        e_cmd("e r mstatus 00")
        result = e_cmd("e c")
        os.environ['BAUD'] = '30'   #48M/0x30=1M
        result = e_cmd(f'e fp "{sys.argv[1]}" 1000 f')

        # #原指令编程
        # result = e_cmd("e 800a 00")
        # os.environ['BAUD'] = '30'   #48M/0x30=1M
        # result = e_cmd(f'e fp "{sys.argv[1]}" 1000')

        #写offset
        result = e_cmd(f"e fw 0 001003 2> null")
        #回读显示固件头
        result = e_cmd("e fr 1000 10 2> null")
        print(result.splitlines()[1])

        #复位
        result = e_cmd("e k")
        print("download finish")

    else:
        print("version:1.0.0")
        print("args:[dat_file_path]")
        print("example:")
        print("python download.py gpio_interrupt.dat")
        print("or download.py gpio_interrupt.dat")
        sys.exit(-1)


if __name__ == "__main__":
    # 当脚本直接运行时，执行主函数
    main()