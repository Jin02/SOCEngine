import sys
import os, os.path
import time
import json

METADATA_FILE_EXTENSION = ".metadata"

# EnumGenerator
def Enum(*sequential, **named):
    enums 	= dict(zip(sequential, range(len(sequential))), **named)
    reverse = dict((value, key) for key, value in enums.iteritems())
    enums['reverse_mapping'] = reverse
    return type('Enum', (), enums)

# First return value is success or fail
# Second return value is replaceString
def ReplaceString(srcStr, fromStr, toStr):
	replaceStr 	= srcStr.replace(fromStr, toStr)
	return fromStr in srcStr, replaceStr

# Remove Tap and Empty Space.
# ex:) struct \t \t \t Test -> struct Test
def CleanUpCode(lineCode):
	while 1: # "struct \t\t\t name" -> "struct     name"
		t, lineCode = ReplaceString(lineCode, '\t', ' ')
		if t == False:
			break
	while 1: # "struct     name" -> "struct name"
		t, lineCode = ReplaceString(lineCode, "  ", ' ')
		if t == False:
			break
	return lineCode

#same struct.
#data in struct likes most D3D11_INPUT_ELEMENT_DESC
class InputLayout:
	semanticName = ''
	semanticIndex = 0
	format = ''				# format only use uint, int, float and matrix.
	usingType = 'VERTEX'	# or INSTANCE
	alignedByteOffset = 0

# main of script.
# hlsl parser
class ParseCode:
	StateEnum 				= Enum('None', 'Comment', 'FindingStructName', 'FindingSemanticVariable')
	isStructArea 			= False	
	isCommentArea 			= False
	currentStructName 		= ""
	currentState 			= 0		#init value is StateEnum.None
	beforeState 			= 0		#init value is StateEnum.None
	structDictionary 		= None
	mainFuncUsingStructName = None
	def __init__(self):
		self.currentState 				= self.StateEnum.None
		self.beforeState 				= self.StateEnum.None
		self.structDictionary 			= dict()
		self.mainFuncUsingStructName 	= dict()
	def __del__(self):
		del self.structDictionary
		del self.mainFuncUsingStructName
	def UpdateState(self, state):
		self.beforeState 	= self.currentState
		self.currentState 	= state
	def Run(self, lineCode):
		def RemoveComment(code):
			commentSplitCode 	= code.split("//")	# remove //
			splitCodeCount 		= len(commentSplitCode)
			modifiedCode 		= commentSplitCode[0]

			if splitCodeCount > 1:
				modifiedCode += '\n'

			#remove /* */
			def RemoveGlobalComment(code):
				while 1: 
					start 	= code.find("/*")
					end 	= code.find("*/")

					if ((start != -1) and (end != -1)) and (start < end):
						code = code[:start] + code[end+2:]
					else:
						break

				return code

			modifiedCode = RemoveGlobalComment(modifiedCode)

			if self.isCommentArea:
				end = modifiedCode.find("*/")
				if end != -1:
					self.UpdateState(self.beforeState)
					self.isCommentArea 	= False
					modifiedCode 		= RemoveGlobalComment( modifiedCode[(end+2):] )
				else:
					modifiedCode = ""
			elif "/*" in modifiedCode: #also isCommentArea == false
				self.UpdateState(self.StateEnum.Comment)
				self.isCommentArea 	= True
				modifiedCode 		= modifiedCode[:start]

			return modifiedCode

		code = RemoveComment(lineCode)

		if (self.currentState == self.StateEnum.FindingStructName) or ('struct' in code):
			self.currentStructName = self.ParseStructName(code)
			self.structDictionary[self.currentStructName] = list()
		elif self.currentState == self.StateEnum.FindingSemanticVariable:			
			self.ParseSemanticVariable(code, self.structDictionary[self.currentStructName])
		else:
			start 	= code.find('(')
			end 	= code.find(')')

			#Find part of func define.
			if end != -1 and start != -1:
				# funcArgumentCodePart is (VS_INPUT input)
				# but does not inclde '(' and ')'
				funcArgumentCodePart = code[start+1:end]
				for structName in self.structDictionary:					
					if structName in funcArgumentCodePart:
						tokens = code[:start].split(' ')
						count = len(tokens)
						for i in xrange(0, count):
							idx = count -i -1
							if idx < 0:
								break
							elif tokens[idx] != '':
								self.mainFuncUsingStructName[ tokens[idx] ] = structName
								break
	def ParseStructName(self, code):
		if self.currentState == self.StateEnum.Comment:
			return

		structName 		= ""
		modifiedCode 	= CleanUpCode( code.replace('\n', '') )
		splitCodeSpace 	= modifiedCode.split(' ') # struct name -> [' ', 'struct', 'name']

		if self.isStructArea == False:
			if "struct" in modifiedCode:

				structPos = 0
				# find 'struct' position
				for codePiece in splitCodeSpace:
					if codePiece == "struct":
						break
					structPos += 1
			
				length = len(splitCodeSpace)
				# find struct name
				for i in xrange(structPos+1, length):
					if splitCodeSpace[i] != '{':
						structName = splitCodeSpace[i]
						break

			# not found
			if structName != "":
				self.UpdateState(self.StateEnum.FindingSemanticVariable)
			else:
				self.UpdateState(self.StateEnum.FindingStructName)

			self.isStructArea = (structName != '')
		else:
			# find struct name
			for name in splitCodeSpace:
				if (name != '') or (name != '{'):
					structName = name
					break

			self.UpdateState(self.StateEnum.FindingSemanticVariable)
			self.isStructArea = False

		return structName
	def ParseSemanticVariable(self, code, layoutList):
		#semancVariableNames = ["POSITION", "BINORMAL", "NORMAL", "TANGENT", "TEXCOORD", "SV_InstanceID", "INSTANCE"]
		modifiedCode 		= CleanUpCode(code)

		if '}' in modifiedCode:
			self.isStructArea = False
			self.UpdateState(self.StateEnum.None)			
			return

		def ParseSemanticName(code):
			colonPos = code.find(":")
			if colonPos == -1:
				return None
			semiColonPos = code.find(";")
			return code[colonPos+1 : semiColonPos].strip()

		semanticIndex 	= 0
		semanticName 	= ParseSemanticName(modifiedCode)
		usingType 		= "VERTEX"
		foundNamePos 	= -1

		if semanticName == None:
			return

		nameLength = len(semanticName)

		wtf = 0
		for si in xrange(nameLength-1, 0, -1):
			wtf = si
			if not semanticName[si:nameLength].isdigit():
				break

		hasSemanticIdx = (nameLength-1) != wtf

		if hasSemanticIdx:
			semanticIndex = int(semanticName[wtf+1:])
			semanticName = semanticName[:wtf+1]
		elif semanticName == "SV_InstanceID": # not write
			return
		elif semanticName == "INSTANCE":
			usingType = "INSTANCE"

		modifiedCode = modifiedCode[:foundNamePos]
		splitCodes 		= modifiedCode.split(' ')

		typeOfVariable 	= ''
		nameOfVariable 	= ''		
		#find type and name
		for codePiece in splitCodes:
			if codePiece != '':
				if typeOfVariable == '':
					typeOfVariable = codePiece
				elif nameOfVariable == '':
					nameOfVariable = codePiece
					break

		def CalculateAlignedByteOffset(layoutList, addUsingType):
			count = len(layoutList)
			if count < 1:
				return 0

			beforeLayout 	= None

			for i in xrange(0, count):
				beforeLayout = layoutList[count - 1 - i]
				if addUsingType == beforeLayout.usingType:
					break

			if beforeLayout.usingType != addUsingType:
				return 0

			beforeFormat = beforeLayout.format

			# foramt = float4 or float2 or int2.
			# so, beforeFormat[len(beforeFormat)-1:] is 4 or 2 etc..
			numTxt = beforeFormat[len(beforeFormat)-1:]

			def isInt(s):
				try:
					int(s)
					return True
				except ValueError:
					return False

			num = 1;

			if isInt(numTxt) == True:
				num = int(numTxt)

			alignedByteOffset = 4 * num + beforeLayout.alignedByteOffset

			if beforeLayout.semanticName == 'POSITION':
				alignedByteOffset -= 4

			return alignedByteOffset		

		def MakeInputLayout(name, variableType, index, offset, usingType):
			layout 						= InputLayout()
			layout.semanticName 		= name
			layout.semanticIndex 		= index
			layout.alignedByteOffset 	= offset
			layout.format 				= variableType
			layout.usingType 			= usingType;
			return layout

		if typeOfVariable == "matrix" or typeOfVariable == "float4x4":
			for i in xrange(0, 4):
				layout = MakeInputLayout(semanticName, "float4", i, CalculateAlignedByteOffset(layoutList, usingType), usingType)
				layoutList.append(layout)
		else:
			layout = MakeInputLayout(semanticName, typeOfVariable, semanticIndex, CalculateAlignedByteOffset(layoutList, usingType), usingType)
			layoutList.append(layout)

		return

####### Run ################################################################################################
WorkReturnValues = Enum('EmptyFile', 'Success', 'NotCreateMeta')
def Work(shaderFilePath, metaDataFilePath, useEasyView):
	shaderFileModifyTime = os.path.getmtime(shaderFilePath)
	print "\nShader File Modify Time : " + time.ctime(shaderFileModifyTime)

	isCreateMetadata = True

	# check original file
	if os.path.isfile(metaDataFilePath):
	 	metadataFile = open(metaDataFilePath, 'rU')
	 	js = json.loads(metadataFile.read())
	 	metadataFile.close()	 	

	 	if len(js) != 0:
		 	#check Date
		 	isCreateMetadata = (js["ShaderFileModifyTime"] != time.ctime(shaderFileModifyTime))

 	#check Shader File
 	shaderFile 	= open(shaderFilePath, 'rU')
 	firstLine 	= shaderFile.readline()
 	shaderFile.close()

 	#check create file
	isCreateMetadata = ((NOT_CREATE_META_DATA in firstLine) == False)

	if isCreateMetadata:		
		if ONLY_PATH_FINDING in firstLine:
			return WorkReturnValues.EmptyFile

		print "Create Metadata\n"
	else:
		return WorkReturnValues.NotCreateMeta

	shaderFile 	= open(shaderFilePath, 'rU')
	parser 		= ParseCode()

	# parser work
	while 1:	
		line = shaderFile.readline()
		parser.Run(line)
		if not line :
			break

	shaderFile.close()

	#remove dumy data
	deleteKeys = list()
	for structName in parser.structDictionary:
		if len(parser.structDictionary[structName]) == 0:
			deleteKeys.append(structName)
	for structName in deleteKeys:
		del parser.structDictionary[structName]	
	
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

	# PS DATA REMOVE
	deleteStructNames = list()
	for structName in parser.structDictionary:
		for item in parser.structDictionary[structName]:
			if item.semanticName == "SV_POSITION":
				deleteStructNames.append(structName)
				break

	for name in deleteStructNames:
		del parser.structDictionary[name]
	del deleteStructNames
	#END

	outData = '{' + nextLine
	outData += tap + QuotationMarks("ShaderFileModifyTime") + ": " + QuotationMarks(time.ctime(shaderFileModifyTime)) + ',' + nextLine

	outData += tap + QuotationMarks("SemanticStructure") + ': {' + nextLine
	structIdx = 0
	for structName in parser.structDictionary:
		layoutList = parser.structDictionary[structName]
		if len(layoutList) == 0:
			continue

		outData += tap*2 + QuotationMarks(structName) + ": {" + nextLine

		index = 0
		for item in layoutList:
			outData += tap*4 + QuotationMarks(str(index)) + ": {" + nextLine

			outData += tap*5 + QuotationMarks("SemanticName") 		+ ": " + QuotationMarks(item.semanticName)	+ ',' + nextLine
			outData += tap*5 + QuotationMarks("SemanticIndex") 		+ ": " + str(item.semanticIndex) 			+ ',' + nextLine
			outData += tap*5 + QuotationMarks("Format") 			+ ": " + QuotationMarks(item.format) 		+ ',' + nextLine
			outData += tap*5 + QuotationMarks("UsingType") 			+ ": " + QuotationMarks(item.usingType) 	+ ',' + nextLine
			outData += tap*5 + QuotationMarks("AlignedByteOffset") 	+ ": " + str(item.alignedByteOffset) 		+ nextLine

			outData += tap*4 + '}' + AddComa(layoutList, index)
			index += 1

		outData += tap*2 + '}' + AddComa(parser.structDictionary, structIdx)
		metaDataFile.write(outData)
		outData = ""
		structIdx += 1
	metaDataFile.write(tap + '}') 

	#mainFunc data write
	mfusCount = len(parser.mainFuncUsingStructName)
	if mfusCount != 0:
		outData = ',' + nextLine + tap + QuotationMarks("MainFunctions") + ": {" + nextLine
		idx = 0
		for mainFuncName in parser.mainFuncUsingStructName:
			idx += 1
			outData += tap*2 + QuotationMarks(mainFuncName) + ":" + QuotationMarks(parser.mainFuncUsingStructName[mainFuncName])
			if mfusCount != idx:
				outData += ',' + nextLine
			else:
				outData += nextLine

		outData += tap + '}'
		metaDataFile.write(outData)

	metaDataFile.write(nextLine + '}')
	metaDataFile.close()
	del parser
	return WorkReturnValues.Success

NOT_CREATE_META_DATA = "NOT_CREATE_META_DATA"
ONLY_PATH_FINDING = "EMPTY_META_DATA"  #Empty Meta Data
CONSOLE_LINE = "***********************************************"

print CONSOLE_LINE + '\n'
print "SOC Framework Shader MetaData Generator\n"

def Dump():
	print "\nParamater Error!!\n"
	print 'Example 1 :'
	print "-ProjectDir ./Test/ -MetaDataCustomTargetDir ./MetaDataFiles/ -IsShaderWithMetaData False -UseEasyView True\n"
	print 'Example 2 :'
	print "-ProjectDir ./ -MetaDataCustomTargetDir NotUsing -IsShaderWithMetaData True -UseEasyView False\n"
	print 'Example 3 :'
	print "-ProjectDir ./ -IsShaderWithMetaData True\n"
	
def CheckParameter():
	scriptRunStartDir 		= 'undefined'
	metaDataCustomTargetDir = 'undefined'
	useEasyView 			= True
	isShaderWithMetaData 	= True

	argvCount = len(sys.argv)-1
	if argvCount >= 4:
		count = 7
		if count > argvCount:
			count = argvCount

		for i in xrange(1, count):
			if sys.argv[i] == '-ProjectDir':
				scriptRunStartDir 		= sys.argv[i+1]
			elif sys.argv[i] == '-IsShaderWithMetaData':
				isShaderWithMetaData 	= (sys.argv[i+1] == 'True')
			elif sys.argv[i] == '-UseEasyView':
				useEasyView 			= (sys.argv[i+1] == 'True')
			elif sys.argv[i] == '-MetaDataCustomTargetDir':
				metaDataCustomtargetDir = sys.argv[i+1]
			else:
				i-=1
			i+=1

	result = True
	if isShaderWithMetaData == False:
		if metaDataCustomTargetDir == 'undefined':
			result = False
	elif scriptRunStartDir == 'undefined':
		result = False

	return result, scriptRunStartDir, metaDataCustomTargetDir, useEasyView, isShaderWithMetaData

result, scriptRunStartDir, metaDataCustomTargetDir, useEasyView, isShaderWithMetaData = CheckParameter()

if result == False:
	Dump()
	print CONSOLE_LINE
	exit()

def SimpleWriteFile(path, content):
	f = open(path, 'w')
	f.write(content)
	f.close()
	return

targetDir = os.path.normpath(scriptRunStartDir)
for (path, dirs, files) in os.walk(targetDir):
    for fileNameWithExtension in files:
    	fileFullPath 	= path + "/" + fileNameWithExtension    	
        extensionPos 	= fileNameWithExtension.rfind('.')        
        fileExtension 	= fileNameWithExtension[extensionPos:]
        fileName 		= fileNameWithExtension[:extensionPos]

        if (fileExtension == '.fx') or (fileExtension == '.hlsl'):
        	print "Found!!!", fileFullPath
        	shaderFilePath = fileFullPath

        	if isShaderWithMetaData:
        		metaDataFilePath = path + "/" + fileName + METADATA_FILE_EXTENSION
        	else:
        		metaDataFilePath = metaDataCustomTargetDir + fileName + METADATA_FILE_EXTENSION
        	result = Work(shaderFilePath, metaDataFilePath, useEasyView)
        	if result == WorkReturnValues.EmptyFile: #ONLY_PATH_FINDING, empty file
        		SimpleWriteFile(metaDataFilePath, "{}")

print "Success!\n"
print CONSOLE_LINE