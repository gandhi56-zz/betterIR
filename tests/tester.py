
import sys
import json
import os

data = {}

def parseJSON(configFile):
    global data
    with open(configFile, 'r') as fileStream:
        data = json.load(fileStream)
        for passName in data:
            run(passName)

def run(passName):
    global data
    inputFile = '/home/anshil/workspace/betterIR/tests/DeadCodeElimination/input/gold.ll'
    outputFile = '/home/anshil/workspace/betterIR/tests/DeadCodeElimination/output/gold.ll'
    cmd = '{} -load-pass-plugin {} -passes="{}" {} -S -o {}'.format(data[passName]['opt'], data[passName]['lib-path'], data[passName]['name'], inputFile, outputFile)
    print('Running {}'.format(cmd))
    os.system(cmd)

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print('Please provide a json file as a commandline argument to run the tester.')
        exit(-1)
    jsonFile = sys.argv[1]
    parseJSON(jsonFile)
