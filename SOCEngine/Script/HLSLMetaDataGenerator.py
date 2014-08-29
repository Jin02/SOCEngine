import sys
import os, os.path
import time
import json

NOT_CREATE_META_DATA = "NOT_CREATE_META_DATA"
CONSOLE_LINE = "***********************************************"

print CONSOLE_LINE

print "SOC FrameWork Shader MetaData Generator"

def Dump():
	print "\nParamater Error!!\n"
	print 'Example 1 :'
	print "-ProjectDir ./Test/ -MetaDataCustomTargetDir ./MetaDataFiles/ -IsShaderWithMetaData False -UseEasyView True\n"
	print 'Example 2 :'
	print "-ProjectDir ./ -MetaDataCustomTargetDir NotUsing -IsShaderWithMetaData True -UseEasyView False\n"
	print CONSOLE_LINE

#parameter!
baseDir = 'undefined'
metaDataCustomTargetDir = 'undefined'
useEasyView = True
isShaderWithMetaData = True

#print sys.argv, len(sys.argv)

if len(sys.argv) < 4:
	Dump()
	exit()

count = 7
if count > len(sys.argv):
	count = len(sys.argv)

for i in xrange(0, count):
	if sys.argv[i] == '-ProjectDir':
		baseDir = sys.argv[i+1]
	elif sys.argv[i] == '-IsShaderWithMetaData':
		isShaderWithMetaData = sys.argv[i+1] == 'True'
	elif sys.argv[i] == '-UseEasyView':
		useEasyView = sys.argv[i+1] == 'True'
	elif sys.argv[i] == '-MetaDataCustomTargetDir':
		metaDataCustomtargetDir = sys.argv[i+1]
	else:
		i-=1
	i+=1

if isShaderWithMetaData == False:
	if metaDataCustomTargetDir == 'undefined':
		Dump()
		exit()
else:
	if baseDir == 'undefined':
		Dump()
		exit()

def enum(*sequential, **named):
    enums = dict(zip(sequential, range(len(sequential))), **named)
    reverse = dict((value, key) for key, value in enums.iteritems())
    enums['reverse_mapping'] = reverse
    return type('Enum', (), enums)
def ReplaceString(srcStr, fromStr, toStr):
	found = srcStr.find(fromStr)
	replaceStr = srcStr.replace(fromStr, toStr)
	return found != -1, replaceStr
def CleanUpCode(code):
	while 1: # "struct \t\t\t name" -> "struct     name"
		t, code = ReplaceString(code, '\t', ' ')
		if t == False:
			break

	while 1: # "struct     name" -> "struct name"
		t, code = ReplaceString(code, "  ", ' ')
		if t == False:
			break
	return code

class InputLayout:
	semanticName = ''
	semanticIndex = 0
	format = ''
	usingType = 'VERTEX'
	alignedByteOffset = 0
class ParseCode:
	StateEnum 			= 0		#enum('None', 'Comment', 'FindingStructName')
	isStructArea 		= False
	isCommentArea 		= False
	currentStructName 	= ""
	currentState 		= 0		#StateEnum.None
	beforeState 		= 0		#StateEnum.None
	structDictionary 	= None
	def __init__(self):
		self.StateEnum = enum('None', 'Comment', 'FindingStructName', 'FindingSemanticVariable')
		self.currentState = self.StateEnum.None
		self.beforeState = self.StateEnum.None
		self.structDictionary = dict()
		return
	def __del__(self):
		del self.structDictionary
		return
	def SetCurrentState(self, state):
		self.beforeState = self.currentState
		self.currentState = state
	def Run(self, lineCode):
		code = self.RemoveComment(lineCode)

		if self.currentState == self.StateEnum.FindingStructName or code.find('struct') != -1:
			self.currentStructName = self.ParseStructName(code)
			self.structDictionary[self.currentStructName] = list()
		elif self.currentState == self.StateEnum.FindingSemanticVariable:			
			self.ParseSemanticVariable(code, self.structDictionary[self.currentStructName])

	def RemoveComment(self, code):
		splitCode = code.split("//")	# remove //
		splitCodeCount = len(splitCode)	
		code = splitCode[0]
		if splitCodeCount > 1:
			code += '\n'

		def RemoveSubComment(code):
			while 1: #remove /**/
				start = code.find("/*")
				end = code.find("*/")

				if start != -1 and end != -1:
					if start < end:
						code = code[:start] + code[(end+2):]
					else:
						break
				else:
					break

			return code

		code = RemoveSubComment(code)

		if self.isCommentArea != True:
			start = code.find("/*")
			if start != -1:
				self.SetCurrentState(self.StateEnum.Comment)
				self.isCommentArea = True
				code = code[:start]
		else:
			end = code.find("*/")
			if end != -1:
				self.SetCurrentState(self.beforeState)
				self.isCommentArea = False
				code = code[(end+2):]
				code = RemoveSubComment(code)
			else:
				code = ""

		return code
	def ParseStructName(self, code):
		if self.currentState == self.StateEnum.Comment:
			return

		code = code.replace('\n', '')

		structName = ""
		if self.isStructArea == False:
			structFound = code.find("struct")
			structName = ""
			if structFound != -1:
				code = CleanUpCode(code)
				splitStr = code.split(' ')

				idx = 0
				for item in splitStr:
					if item == "struct":
						break
					idx += 1
			
				splitStrNum = len(splitStr)
				for i in xrange(idx+1, splitStrNum):
					if splitStr[i] != '{':
						structName = splitStr[i]
						break	

			if structName != '':
				self.SetCurrentState(self.StateEnum.FindingSemanticVariable)
			else:
				self.SetCurrentState(self.StateEnum.FindingStructName)
			self.isStructArea = structName != ''
		else:
			code = CleanUpCode(code)
			splitCodes = code.split(' ')
			for name in splitCodes:
				if name != '' or name != '{':
					structName = name
					break

			self.SetCurrentState(self.StateEnum.FindingSemanticVariable)
			self.isStructArea = False
		return structName
	def ParseSemanticVariable(self, code, layoutList):
		semancVariableNames = ["POSITION", "NORMAL", "TANGENT", "BINORMAL", "TEXCOORD", "SV_InstanceID"]
		code = CleanUpCode(code)

		if code.find('}') != -1:
			self.isStructArea = False
			self.SetCurrentState(self.StateEnum.None)			
			return

		semanticIndex = 0
		semanticName = ""
		foundNamePos = -1
		for name in semancVariableNames:
			foundNamePos = code.find(name)
			if foundNamePos != -1:
				semanticName = name
				break
		if semanticName == "TEXCOORD":
			for i in xrange(0,10):
				if code.find(semanticName+str(i)) != -1:
					semanticName = semanticName
					semanticIndex = i
					break
		elif semanticName == "SV_InstanceID":
			self.structDictionary[self.currentStructName].insert(0, "Instance")
			#print "!!!Instance!!!"
			return

		code = code[:foundNamePos]
		if semanticName == '':
			return

		typeNames = ["float", "int", "unit"] #if other types required, when will add other types in that time.

		splitCodes = code.split(' ')
		typeOfVariable = ''
		nameOfVariable = ''
		for t in splitCodes:
			if t != '':
				if typeOfVariable == '':
					typeOfVariable = t
				else:
					if nameOfVariable == '':
						nameOfVariable = t
					break

		def CalculateAlignedByteOffset(layoutList):
			count = len(layoutList)
			if count == 0:
				return 0

			beforeElementOfList = layoutList[count-1]
			beforeFormat = beforeElementOfList.format
			number = beforeFormat[len(beforeFormat)-1:]
			alignedByteOffset = 4 * int(number) + beforeElementOfList.alignedByteOffset
			if beforeElementOfList.semanticName == 'POSITION':
				alignedByteOffset -= 4
			return alignedByteOffset

		layout = InputLayout()
		layout.semanticName = semanticName
		layout.beforeFormat = typeOfVariable
		layout.semanticIndex = semanticIndex
		layout.alignedByteOffset = CalculateAlignedByteOffset(layoutList)
		layout.format = typeOfVariable

		count = len(layoutList)
		layoutList.insert(count, layout)
		#print "Current StructName :", self.currentStructName, "type :", typeOfVariable, "name :", nameOfVariable, "semanticName :", semanticName
		return

####### Run ################################################################################################

def Work(shaderFilePath, metaDataFilePath, useEasyView):
	shaderFileModifyTime = os.path.getmtime(shaderFilePath)
	print "Shader File Modify Time : " + time.ctime(shaderFileModifyTime)

	isCreateMetadata = True
	if os.path.isfile(metaDataFilePath) == True:
	 	f = open(metaDataFilePath, 'rU')
	 	js = json.loads(f.read())
	 	f. close()
	 	
	 	#check Date
	 	isCreateMetadata = js["ShaderFileModifyTime"] != time.ctime(shaderFileModifyTime)

 	#check Shader File
 	f = open(shaderFilePath, 'rU')
 	firstLine = f.readline()
 	f.close()
 	if firstLine.find(NOT_CREATE_META_DATA) != -1:
		isCreateMetadata = False

	if isCreateMetadata == False:
		return
	else:
		print "Create Metadata"

	shaderFile = open(shaderFilePath, 'rU')
	parser = ParseCode()
	print parser

	while 1:	
		line = shaderFile.readline()
		parser.Run(line)
		if not line : 
			break

	shaderFile.close()

	#insert instancing data
	for item in parser.structDictionary:
		if len(parser.structDictionary[item]) > 0:
			if type(parser.structDictionary[item][0]) == str:
				if parser.structDictionary[item][0] == "Instance":
					layoutList = parser.structDictionary[item]
					for i in xrange(0, 4):
						data = InputLayout()
						data.semanticName = "MATRIX"
						data.semanticIndex = i
						data.usingType = 'INSTANCE'
						data.format = 'float4'
						data.alignedByteOffset = i * 16 # 16 = float4
						layoutList.insert(len(layoutList), data)
					layoutList.remove("Instance")

	#############    metaData    ################################################################################

	metaDataFile = open(metaDataFilePath, 'w')

	nextLine = '\n'
	tap = '\t'

	if useEasyView == False:
		nextLine = ''
		tap = ''

	def AddComa(component, index):
		if index != len(component) -1:
			return ',' + nextLine
		return nextLine
	def QuotationMarks(code):
		code = "\"" + code +"\""
		return code

	outData = '{' + nextLine
	outData += tap + QuotationMarks("ShaderFileModifyTime") + ": " + QuotationMarks(time.ctime(shaderFileModifyTime)) + ',' + nextLine

	structIdx = 0
	for structName in parser.structDictionary:
		outData += tap + QuotationMarks(structName) + ": {" + nextLine
		#print tap + "/" + QuotationMarks(structName)# + ": {" + nextLine
		index = 0
		layoutList = parser.structDictionary[structName]

		for item in layoutList:
			outData += tap*2 + QuotationMarks(str(index)) + ": {" + nextLine

			outData += tap*3 + QuotationMarks("SemanticName") 		+ ": " + QuotationMarks(item.semanticName)	+ ',' + nextLine
			outData += tap*3 + QuotationMarks("SemanticIndex") 		+ ": " + str(item.semanticIndex) 			+ ',' + nextLine
			outData += tap*3 + QuotationMarks("Foramt") 			+ ": " + QuotationMarks(item.format) 		+ ',' + nextLine
			outData += tap*3 + QuotationMarks("UsingType") 			+ ": " + QuotationMarks(item.usingType) 	+ ',' + nextLine
			outData += tap*3 + QuotationMarks("AlignedByteOffset") 	+ ": " + str(item.alignedByteOffset) 		+ nextLine

			outData += tap*2 + '}' + AddComa(layoutList, index)
			index += 1

		outData += tap + '}' + AddComa(parser.structDictionary, structIdx)
		metaDataFile.write(outData)
		outData = ""
		structIdx += 1

	metaDataFile.write('}')
	metaDataFile.close()
	del parser
	return

targetDir = os.path.normpath(baseDir)
for (path, dirs, files) in os.walk(targetDir):
    for fileNameWithExtension in files:
    	fileFullPath = path + "/" + fileNameWithExtension    	
        extensionPos = fileNameWithExtension.rfind('.')
        
        fileExtension = fileNameWithExtension[extensionPos:]
        fileName = fileNameWithExtension[:extensionPos]

        if fileExtension == '.fx' or fileExtension == '.hlsl':
        	print "Found!!!", fileFullPath
        	shaderFilePath = fileFullPath
        	if isShaderWithMetaData == True:
        		metaDataFilePath = path + "/" + fileName + ".metadata"
        	else:
        		metaDataFilePath = metaDataCustomTargetDir + fileName + ".metadata"
        	Work(shaderFilePath, metaDataFilePath, useEasyView)

print "Success!"
print CONSOLE_LINE