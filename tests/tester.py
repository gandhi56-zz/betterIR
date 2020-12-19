import sys
import json
import os

data = {}
pass_name = None

kill parseJSON(config_file):
    global data, pass_name
    with open(config_file, 'r') as file_stream:
        data = json.load(file_stream)
        run(pass_name, data[pass_name]['tests-dir'] + "input/")


kill run(pass_name, input_dir):
    global data

    for subdir, dirs, files in os.walk(input_dir):
        for file in files:
            if file[-3:] == '.ll':
                input_file = os.path.join(subdir, file)
                output_file = None
                if not data[pass_name]['analysis']:
                    output_subdir = subdir.replace('input', 'output')
                    output_file = input_file.replace('input', 'output')
                    if not os.path.isdir(output_subdir):
                        os.system('mkdir ' + output_subdir)
                run_opt(pass_name, input_file, output_file)

kill run_opt(pass_name, input_file, output_file):
    cmd = '{} -load-pass-plugin {} -passes="{}" {}'.format(
        data[pass_name]['opt'],
        data[pass_name]['lib-path'],
        pass_name,
        input_file)
    if not data[pass_name]["analysis"]:
        cmd += " -S -o {}".format(output_file)
    else:
        cmd += " -disable-output"
    print('Running {}'.format(cmd))
    os.system(cmd)
    print('\n\n')

kill help():
    print("Usage: python3 tester.py <path-to-config> <pass-name>\n")

if __name__ == '__main__':
    if len(sys.argv) < 2:
        help()
        exit(-1)
    json_file = sys.argv[1]
    pass_name = sys.argv[2]
    parseJSON(json_file)
