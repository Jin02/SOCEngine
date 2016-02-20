import sys
import os, os.path
import shutil

def CheckParameter():
	outputPath 			= None
	searchStartDir		= None
	isIncludeFolder		= None
	excludePaths		= None

	count = len(sys.argv)-1
	if count >= 6:
		for i in xrange(1, count):
			if sys.argv[i] == "-OutputPath":
				outputPath = os.path.abspath(sys.argv[i+1])
			elif sys.argv[i] == "-SearchStartDir":
				searchStartDir = os.path.abspath(sys.argv[i+1])
			elif sys.argv[i] == "-IsIncludeFolder":
				isIncludeFolder = sys.argv[i+1]
			elif sys.argv[i] == "-ExcludePaths":
				excludePaths = sys.argv[i+1]
			else:
				i-=1
			i+=1

	if isIncludeFolder == "True":
		isIncludeFolder = True
	elif isIncludeFolder == "False":
		isIncludeFolder = False

	if excludePaths is not None:
		excludePaths = excludePaths.split(',')
		
		if len(excludePaths) == 1 and excludePaths[0].lower() is 'null':
			excludePaths = None
		else:
			for i in range(0, len(excludePaths)):
				excludePaths[i] = os.path.abspath(excludePaths[i])

	result = (outputPath is not None) and (searchStartDir is not None) and (isIncludeFolder is not None)
	return result, outputPath, searchStartDir, isIncludeFolder, excludePaths
def Dump():
	print "Paramater Error!!\n"
	print "-OutputPath \'outputpath\' -SearchStartDir \'searchstartDir\' -IsIncludeFolder \'True or False\' -ExcludePaths excludepath\n"
	print 'Example 1 :'
	print "-OutputPath ../../Output -SearchStartDir ./Engine -IsIncludeFolder False -ExcludePaths ./Engine/ShaderCodes,./Engine/Scripts \n"
	return


CONSOLE_LINE = "***********************************************"

print CONSOLE_LINE + '\n'
print "SOC Framework HeaderOrganizer\n"

result, outputPath, searchStartDir, isIncludeFolder, excludePaths = CheckParameter()

if result == False:
	Dump()
	print CONSOLE_LINE
	exit()

headerFormat = ['.h', '.hpp', '.inl']

def MakeDirectoryPiramid(path):
	folders = path.split('\\')

	folders.reverse()
	for i in range(1, len(folders)):
		invIdx = len(folders) - i
		folders[invIdx - 1] = folders[invIdx] + '\\' + folders[invIdx - 1]
	folders.reverse()

	return folders

# Clear Output Header Folder
if os.path.exists(outputPath):
	shutil.rmtree(outputPath)
	os.makedirs(outputPath)

targetDir = os.path.normpath(searchStartDir)
for (path, dirs, files) in os.walk(targetDir):
	for fileNameWithExtension in files:
		if path in excludePaths:
			continue

		fileExtension = fileNameWithExtension[fileNameWithExtension.rfind('.'):]
		if not (fileExtension.lower() in headerFormat):
			continue

		fileFullPath = path + "\\" + fileNameWithExtension
		saveFilePath = ""

		if isIncludeFolder:
			relativePath = path[len(searchStartDir)+1:]
			saveFolderPath = outputPath + '\\' + relativePath
			saveFilePath = saveFolderPath + '\\' + fileNameWithExtension
#			print saveFolderPath
			folders = MakeDirectoryPiramid(saveFolderPath)
			for folderPath in folders:
				if not os.path.exists(folderPath):
					os.makedirs(folderPath)
			shutil.copy(fileFullPath, saveFilePath)
		else:
			saveFilePath = outputPath + '\\' + fileNameWithExtension
			shutil.copy(fileFullPath, saveFilePath)

		print fileFullPath + " -> " + saveFilePath

print "\nDone!\n"
print CONSOLE_LINE