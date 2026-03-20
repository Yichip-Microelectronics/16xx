import os
import sys
import json
import yaml
import re
import shutil


def is_hex_string(input_string:str) -> bool:
    hex_pattern = re.compile(r'^[0-9a-fA-F]+$')
    return bool(hex_pattern.match(input_string))

def get_eide_prj_define_dict(obj_path:str) -> dict:
    eide_define_dict = {}
    if obj_path.find("vscode_eide") == -1:
        return eide_define_dict
    else:
        obj_path_directory = os.path.dirname(obj_path)# 获取obj目录
        eide_prj_file_path = os.path.join(obj_path_directory, os.pardir, os.pardir, ".eide", "eide.json")# obj目录回退两级拼接eide工程文件目录
        eide_prj_file_path = os.path.normpath(eide_prj_file_path)#优化路径回退符
        if os.path.exists(eide_prj_file_path):#适配老版本eide配置文件eide.json
            with open(eide_prj_file_path, 'r') as eidePrjFile:
                eide_data = json.load(eidePrjFile)
                eide_define_list = eide_data['targets']['Debug']['custom_dep']['defineList']
                for item in eide_define_list:
                    # 使用in关键字检查字符串是否包含等号
                    if '=' in item:
                        key, value = item.split('=', 1)
                        eide_define_dict[key] = value
                    else:
                        eide_define_dict[item] = ''
        else:#适配新版本eide配置文件eide.yml
            eide_prj_file_path = os.path.join(obj_path_directory, os.pardir, os.pardir, ".eide", "eide.yml")# obj目录回退两级拼接eide工程文件目录
            eide_prj_file_path = os.path.normpath(eide_prj_file_path)#优化路径回退符
            with open(eide_prj_file_path, 'r') as eidePrjFile:
                eide_data = yaml.safe_load(eidePrjFile)
                eide_define_list = eide_data['targets']['Debug']['cppPreprocessAttrs']['defineList']
                for item in eide_define_list:
                    # 使用in关键字检查字符串是否包含等号
                    if '=' in item:
                        key, value = item.split('=', 1)
                        eide_define_dict[key] = value
                    else:
                        eide_define_dict[item] = ''


        return eide_define_dict

def get_branch_include_dict(config_file_path:str) -> dict:
    branch_include_dict = {}# 存储匹配结果的字典
    with open(config_file_path, 'r',encoding='utf-8') as configFile:
        configFileValue = configFile.read()
        pattern =   r'#if\s*\(BT_CODE_BRANCH_SELECT\s*==\s*(\w+)\)\s*#include\s*"(.*?)"|' \
                    r'#elif\s*\(BT_CODE_BRANCH_SELECT\s*==\s*(\w+)\)\s*#include\s*"(.*?)"|' \
                    r'#else\s*#include\s*"(.*?)"'

        matches = re.finditer(pattern, configFileValue, re.DOTALL)

        for match in matches:
            selected_branch = match.group(1) or match.group(3) or 'else'
            include_path = match.group(2) or match.group(4) or match.group(5)
            branch_include_dict[selected_branch] = include_path

    if not branch_include_dict:
        print("branch_include_dict is empty!")

    return branch_include_dict

'''
功能:获取c文件宏定义的值
参数:c文件路径 宏定义名称
返回:指定宏定义的值
'''
def get_define_value(file_path:str, define_name:str) -> str:
    with open(file_path, 'r',encoding='utf-8') as file:
        fileData = file.read()
        pattern = r'^\s*#define\s+'+ re.escape(define_name)+r'\s+(.*)'
        match_define_value = re.search(pattern, fileData,re.MULTILINE)# 使用正则表达式匹配宏定义的值
        if match_define_value:
            return match_define_value.group(1)
        else:
            print(file_path," ",define_name," not define!")
            return ""

'''
功能:获取c文件宏定义组字典(传入前缀"BOARD_"将返回匹配的键值对[字典])
参数:c文件路径 宏定义组名称前缀
返回:宏定义名称与值构成的字典
'''
def get_define_group_dict(file_path:str, define_prefix:str) -> dict:
    result_dict = {}

    with open(file_path, 'r',encoding='utf-8') as file:
        fileData = file.read()
        pattern = re.compile(r'#define\s+({}\w+)\s+(.*)'.format(re.escape(define_prefix)))
        matches = pattern.findall(fileData)
        for match in matches:
            define_name, define_value = match
            result_dict[define_name] = define_value

    return result_dict


def get_bt_branch_directory(config_file_path:str, eide_define_dict:dict) -> str:
    branch_include_dict = get_branch_include_dict(config_file_path)
    bt_branch_define = eide_define_dict.get("BT_CODE_BRANCH_SELECT","")
    if bt_branch_define == "":
        bt_branch_define = get_define_value(config_file_path,"BT_CODE_BRANCH_SELECT")

    bt_branch_directory = branch_include_dict.get(bt_branch_define,"")
    if bt_branch_directory == "":
        bt_branch_directory = branch_include_dict.get("else","")# #else分支

    if bt_branch_directory != "":
        config_file_directory = os.path.dirname(config_file_path)# 获取config_file目录
        bt_branch_directory = os.path.join(config_file_directory, bt_branch_directory)#拼接蓝牙分支绝对路径
        bt_branch_directory = os.path.normpath(bt_branch_directory)#优化路径回退符
        bt_branch_directory = os.path.dirname(bt_branch_directory)
    else:
        print("bt_branch_directory is empty!")

    return bt_branch_directory

def gen_io_config_file(config_file_path:str, eide_define_dict:dict) -> bool:
    board_type = eide_define_dict.get("BOARD_TYPE","")
    if board_type == "":
        board_type = get_define_value(config_file_path,"BOARD_TYPE")
        if board_type == "":
            return False

    board_define_dict = get_define_group_dict(config_file_path,"BOARD_")
    board_define_value = board_define_dict.get(board_type,"")
    if board_define_value == "":
        print(config_file_path," ",board_type," not define!")
        return False

    board_define_value = board_define_value.replace('(', '').replace(')', '')
    board_define_value = board_define_value.lower()
    if board_define_value[:2] == "0x":
        board_define_value = board_define_value[2:]

    if is_hex_string(board_define_value) == False:
        print(board_type,"value is not hex string->",board_define_value)
        return False
    io_config_list = [board_define_value[i:i+2] for i in range(0, len(board_define_value), 2)]
    io_config_file_path = os.path.dirname(os.path.abspath(sys.argv[0]))
    io_config_file_path = os.path.join(io_config_file_path, "io_config.txt")
    with open(io_config_file_path, 'w') as ioConfigFile:
        io_fun_code = 0x02  #GPIO_MODE_QSPI_NCS
        for item in io_config_list:
            if int(item,16) < 0x80:   #io功能寄存器地址低位
                ioConfigFile.write("e 83"+ item + " " + hex(io_fun_code)[2:].zfill(2)+"\n")
                io_fun_code += 1
            if io_fun_code > 0x07:  #GPIO_MODE_QSPI_IO3
                break

def main():
    if len(sys.argv)>=3:
        rv_bin_path = sys.argv[1]
        config_file_path = sys.argv[2]

        eide_define_dict = get_eide_prj_define_dict(rv_bin_path)

        #gen io config file
        if False == gen_io_config_file(config_file_path,eide_define_dict):
            sys.exit(-1)

        #gen dat
        # crc_check_en:0 is disable code crc check,other is enable code crc check
        # otp_key and flash_iv:null is disable code enc,other is code enc key, otp_key and flash_iv is 16 bytes hex string
        crc_check_en="1"
        otp_key=""
        flash_iv=""
        util_dir = os.path.dirname(os.path.abspath(sys.argv[0]))
        gendat_path = os.path.join(util_dir, "gendat.exe")
        file_base, file_extension = os.path.splitext(rv_bin_path)
        out_file_path = file_base + '.dat'
        cmd = f'""{gendat_path}" "{rv_bin_path}" "{out_file_path}" {crc_check_en} {otp_key} {flash_iv} > nul"'
        exit_code = os.system(cmd)
        if exit_code != 0:
            sys.exit(-1)
        if not os.path.exists(out_file_path):
            print("gen ",out_file_path," fail!")
            sys.exit(-1)

        sys.exit(0)


    else:
        print("version:1.0.1")
        print("args:[rv_code_bin_file_path] [congfig_file_path]")
        print("example:")
        print("python gendat.py gpio_interrupt.bin board_config.h")
        print("or gendat.exe gpio_interrupt.bin board_config.h")
        sys.exit(-1)


if __name__ == "__main__":
    # 当脚本直接运行时，执行主函数
    main()