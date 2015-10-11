import sys
import os, os.path
import subprocess

def CheckParameter():
	fbxConvPath 		= None
	outputType			= None
	runDir				= None

	count = len(sys.argv)-1
	if count >= 6:
		for i in xrange(1, count):
			if sys.argv[i] == "-FBXConvPath":
				fbxConvPath = sys.argv[i+1]
			elif sys.argv[i] == "-OutputType":
				outputType = sys.argv[i+1]
			elif sys.argv[i] == "-RunStartDir":
				runDir = sys.argv[i+1]
			else:
				i-=1
			i+=1

	OutputTypes = ['json', 'binary']
	if not outputType in OutputTypes:
		outputType = None

	result = (fbxConvPath is not None) and (outputType is not None) and (runDir is not None)
	return result, fbxConvPath, outputType, runDir
def Dump():
	print "Paramater Error!!\n"
	print "-FBXConvPath \"Your Fbx-conv exe path\" -OutputType json or binary -RunStartDir \"Your Path\"\n"
	print 'Example 1 :'
	print "-FBXConvPath ../Tool/fbx-conv.exe -OuputType json -RunStartDir ../../SOCTestProj/Resources\n"
	print "OutputTypes only support the json, binary\n"
	return


CONSOLE_LINE = "***********************************************"

print CONSOLE_LINE + '\n'
print "SOC Framework FBX-Conv Luncher\n"

if not 'FBX_SDK_ROOT' in os.environ:
	print "ERROR: Environment variable FBX_SDK_ROOT is not set."
	print "Set it to something like: C:\\Program Files\\Autodesk\\FBX\\FBX SDK\\2013.3"
	exit()

result, fbxConvPath, outputType, runStartDir = CheckParameter()
if result == False:
	Dump()
	print CONSOLE_LINE
	exit()

if outputType == 'json':
	outputType = 'g3dj'
elif outputType == 'binary':
	outputType = 'g3db'
else:
	assert("invalid output type.")

supported3DFileFormats = ['.obj', '.fbx', '.dae']

targetDir = os.path.normpath(runStartDir)
for (path, dirs, files) in os.walk(targetDir):
	for fileNameWithExtension in files:
		fileExtension = fileNameWithExtension[fileNameWithExtension.rfind('.'):]

		if not (fileExtension.lower() in supported3DFileFormats):
			continue

		fileName = fileNameWithExtension[:fileNameWithExtension.find('.')]
		fileFullPath = path + "\\" + fileNameWithExtension

		outputFileNameWithExtension = fileName + "_" + fileExtension[1:] + '.' + outputType
		outputFilePath = fileFullPath[:fileFullPath.rfind('\\')] + '\\' + outputFileNameWithExtension

		if os.path.isfile(outputFilePath):
			continue

		command = fbxConvPath + " -f " + fileFullPath + ' ' + outputFilePath

		res = subprocess.check_output(command, shell=True)
		print "Generate File : " + outputFileNameWithExtension

print "Done!\n"
print CONSOLE_LINE