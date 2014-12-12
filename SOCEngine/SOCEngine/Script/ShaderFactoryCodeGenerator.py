import sys
import os, os.path
import json

def tap(num):
	return '\t'*num
def nextLine(num):
	return '\n'*num
class ShaderFactory:
	originFileDir = None
	saveDir = None
	addCodeBeginCommand = "/** Script Begin **/"
	addCodeEndCommand = "/** Script End **/"
	def __init__(self, originFileDir, saveDir):
		self.originFileDir 	= originFileDir
		self.saveDir 		= saveDir
		return
	def Run(self, code, className):
		factoryFile = open(self.originFileDir, 'rU')
		source 		= factoryFile.read()
		factoryFile.close()

		source = source.replace("[ClassName]", className)

		begin 		= source.find(self.addCodeBeginCommand) + len(self.addCodeBeginCommand)
		end 		= source.find(self.addCodeEndCommand)
		source 		= source[:begin] + code + source[end:]
		factoryFile = open(self.saveDir, 'w')
		factoryFile.write(source)
		factoryFile.close()
def isInt(s):
  try:
    int(s)
    return True
  except ValueError:
    return False
def readJson(filename):
	f = open(filename, 'r')
	js = json.loads(f.read())
	f.close()
	return js
def QuotationMarks(code):
	code = "\"" + code +"\""
	return code
def CheckParameter():
	originFactory 		= None
	saveFactory 		= None
	runDir				= None
	className 			= None

	count = len(sys.argv)-1
	if count >= 6:
		for i in xrange(1, count):
			if sys.argv[i] == "-OriginalShaderFactoryFile":
				originFactory = sys.argv[i+1]
			elif sys.argv[i] == "-SaveShaderFactoryFile":
				saveFactory = sys.argv[i+1]
			elif sys.argv[i] == "-ScriptRunStartDir":
				runDir = sys.argv[i+1]
			elif sys.argv[i] == "-ClassName":
				className = sys.argv[i+1]
			else:
				i-=1
			i+=1

	if className == None:
		className = "Factory"

	result = (originFactory != None and saveFactory != None and runDir != None)
	return result, originFactory, saveFactory, runDir, className
def Dump():
	print "\nParamater Error!!\n"
	print 'Example 1 :'
	print "-OriginalShaderFactoryFile ./origin.hpp -SaveShaderFactoryFile ./save.hpp -ScriptRunStartDir ./Shader -ClassName Factory\n"

CONSOLE_LINE = "***********************************************"

print CONSOLE_LINE + '\n'
print "SOC Framework ShaderFactoryCode Generator"

result, originalShaderFactoryFileDir, saveShaderFactoryFileDir, scriptRunStartDir, className = CheckParameter()
if result == False:
	Dump()
	print CONSOLE_LINE
	exit()

code = ""
targetDir = os.path.normpath(scriptRunStartDir)
for (path, dirs, files) in os.walk(targetDir):
    for fileNameWithExtension in files:
    	fileFullPath = path + "\\" + fileNameWithExtension    	
        extensionPos = fileNameWithExtension.rfind('.')
        
        fileExtension = fileNameWithExtension[extensionPos:]
        fileName = fileNameWithExtension[:extensionPos]

        if fileExtension != ".metadata":
        	continue

        path = path.replace("\\", "/")
        jsonData = readJson(fileFullPath)

        mainFuncs = dict()
        for structName in jsonData["SemanticStructure"]:
        	mainFuncs[structName] = list()

        for mainFunc in jsonData["MainFunctions"]:
        	structName = jsonData["MainFunctions"][mainFunc]
        	if (structName in mainFuncs) == True:
        		mainFuncs[structName].append(mainFunc)

        #write Code
        #write Code
        #write Code
        code += nextLine(1) + tap(4)+ 'if(shaderName == \"' + fileName + '\")' + nextLine(1)+tap(4)+"{" +nextLine(1)
        code += tap(5) + "folderPath = \""+ path +"/\";" + nextLine(1)

        for struct in jsonData["SemanticStructure"]:
        	if struct in mainFuncs == False:
        		continue
   	        count = len(mainFuncs[struct])
   	        if count == 0:
   	        	continue

   	        code += tap(5) + "if(mainVSFuncName == " + QuotationMarks(mainFuncs[struct][0])
	       	for i in xrange(1, count):
   	       		code += "|| mainVSFuncName == " + QuotationMarks(mainFuncs[struct][i])
   	        code += ")" + nextLine(1)
   	        code += tap(5) + "{" + nextLine(1)

        	for idx in xrange(0, len(jsonData["SemanticStructure"][struct])):
        		element = jsonData["SemanticStructure"][struct][str(idx)]

		        # Make InputSlotClass
		    	inputSlotClass = "D3D11_INPUT_PER_"
		    	inputSlot = 0
		    	instanceDataStepRate = 0
		        if element["UsingType"] == "VERTEX":
		        	inputSlotClass += "VERTEX_DATA"
		        	#inputSlot = 0, instanceDataStepRate = 0
		        elif element["UsingType"] == "INSTANCE":
		        	inputSlotClass += "INSTANCE_DATA"
		        	inputSlot = 1
		        	instanceDataStepRate = 1
		        # end
		        # Make Foramt
		        formatSize = element["Format"][len(element["Format"])-1]
		        format = "DXGI_FORMAT_"
		        if formatSize == '1' or isInt(formatSize) == False:
		        	format += "R32_"
		        elif formatSize == '2':
		        	format += "R32G32_"
		        elif formatSize == '3' or element["SemanticName"] == "POSITION":
		        	format += "R32G32B32_"     	
		        elif formatSize == '4':
		        	format += "R32G32B32A32_"

		        dataType = element["Format"][0]
		        if dataType == 'f': #float
		        	format += "FLOAT"
		        elif dataType == 'u': #uint
		        	format += "UINT"
		        elif dataType == 'i': #int
		        	format += "INT"
		        #end

		        #name, index, format, offset, slotClass, slot, steprate 
		        code += tap(6)
		        code += "AddInputElementDesc(" + '\"' + element["SemanticName"] + '\"' + ',' 
	        	code += str(element["SemanticIndex"]) + ',' + format + ',' + str(element["AlignedByteOffset"]) + ','
		        code += inputSlotClass + ',' + str(inputSlot) + ',' + str(instanceDataStepRate) + ");"
		        code += nextLine(1)

		code += tap(5) + '}' + nextLine(1)
        code += tap(4) + '}' + nextLine(1) + tap(4)

        for structName in jsonData["SemanticStructure"]:
        	del mainFuncs[structName]
        del mainFuncs

shaderFactory = ShaderFactory(originalShaderFactoryFileDir, saveShaderFactoryFileDir)
shaderFactory.Run(code, className)

print "Success!\n"
print CONSOLE_LINE