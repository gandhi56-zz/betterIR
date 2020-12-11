import sys
import json
import os

data = {}
pass_name = None

def parseJSON(config_file):
    global data, pass_name
    with open(config_file, 'r') as file_stream:
        data = json.load(file_stream)
        run(pass_name, data[pass_name]['tests-dir'] + "input/", data[pass_name]['tests-dir'] + 'output/')


def run(pass_name, input_dir, output_dir):
    global data

    for subdir, dirs, files in os.walk(input_dir):
        for file in files:
            if file[-3:] == '.ll':
                input_file = os.path.join(subdir, file)
                output_subdir = subdir.replace('input', 'output')
                output_file = input_file.replace('input', 'output')
                if not os.path.isdir(output_subdir):
                    os.system('mkdir ' + output_subdir)
                run_opt(pass_name, input_file, output_file)

def run_opt(passName, inputFile, outputFile):
    global pass_name
    cmd = '{} -load-pass-plugin {} -passes="{}" {} -S -o {}'.format(
        data[passName]['opt'],
        data[passName]['lib-path'],
        pass_name,
        inputFile, outputFile)
    print('Running {}'.format(cmd))
    os.system(cmd)
    print('\n\n')

def help():
    print("Usage: python3 tester.py <path-to-config> <pass-name>\n")

if __name__ == '__main__':
    if len(sys.argv) < 2:
        help()
        exit(-1)
    json_file = sys.argv[1]
    pass_name = sys.argv[2]
    parseJSON(json_file)
