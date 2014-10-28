import sys
import os, os.path
import json


shaderFactoryFileDir = "D:/Dropbox/test/ShaderFactory.h"
scriptRunStartDir = "D:/Dropbox/test/"

def tap(num):
	return '\t'*num

def nextLine(num):
	return '\n'*num


class ShaderFactory(object):
	fileDir = None
	addCodeBeginCommand = "/** Script Begin **/"
	addCodeEndCommand = "\t\t/** Script End **/"
	def __init__(self, fileDir):
		self.fileDir = fileDir
		return
	def Run(self, code):
		factoryFile = open(self.fileDir, 'rU')
		source = factoryFile.read()
		factoryFile.close()		
		begin = source.find(self.addCodeBeginCommand) + len(self.addCodeBeginCommand)
		end = source.find(self.addCodeEndCommand)
		source = source[:begin] + code + source[end:]
		factoryFile = open(self.fileDir, 'w')
		factoryFile.write(source)
		factoryFile.close()

def isNumber(s):
  try:
    float(s)
    return True
  except ValueError:
    return False

def readJson(filename):
	f = open(filename, 'r')
	js = json.loads(f.read())
	f.close()
	return js

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

        code += nextLine(1) + tap(2)+ 'if(shaderName == \"' + fileName + '\")' + nextLine(1)+tap(2)+"{" +nextLine(1)
        code += tap(3) + "folderPath = \""+ path +"/\";" + nextLine(1)

        for struct in jsonData["SemanticStructure"]:
        	for order in jsonData["SemanticStructure"][struct]:
        		element = jsonData["SemanticStructure"][struct][order]
		        code += tap(3) + "{" + nextLine(1)
		        #code += tap(4) + "D3D11_INPUT_ELEMENT_DESC desc;" + nextLine(1)
		        code += tap(4) + "desc.SemanticName = \"" + element["SemanticName"] +"\";" + nextLine(1)
		        code += tap(4) + "desc.SemanticIndex = " + str(element["SemanticIndex"]) + ";" + nextLine(1)
		        code += tap(4) + "desc.AlignedByteOffset = " + str(element["AlignedByteOffset"]) + ";" + nextLine(1)

		        dataTypeStr = "DXGI_FORMAT_"
		        dataSize = element["Format"][len(element["Format"])-1]

		        if dataSize == '1' or isNumber(dataSize) == False:
		        	dataTypeStr += "R32_"
		        elif dataSize == '2':
		        	dataTypeStr += "R32G32_"
		        elif dataSize == '3' or element["SemanticName"] == "POSITION":
		        	dataTypeStr += "R32G32B32_"     	
		        elif dataSize == '4':
		        	dataTypeStr += "R32G32B32A32_"

		        dataType = element["Format"][0]
		        if dataType == 'f': #float
		        	dataTypeStr += "FLOAT"
		        elif dataType == 'u':
		        	dataTypeStr += "UINT"
		        elif dataType == 'i':
		        	dataTypeStr += "INT"

		        code += tap(4) + "desc.Format = " + dataTypeStr + ";" + nextLine(1)
		        
		        code += tap(4) + "desc.InputSlotClass = D3D11_INPUT_PER_"
		        if element["UsingType"] == "VERTEX":
		        	code += "VERTEX_DATA;" + nextLine(1)
		        	code += tap(4) + "desc.InputSlot = 0;" + nextLine(1)
		        	code += tap(4) + "desc.InstanceDataStepRate = 0;" +nextLine(1)

		        elif element["UsingType"] == "INSTANCE":
		        	code += "INSTANCE_DATA;" + nextLine(1)
		        	code += tap(4) + "desc.InputSlot = 1;" + nextLine(1)
		        	code += tap(4) + "desc.InstanceDataStepRate = 1;" +nextLine(1)

		        code += tap(4) + "vertexDeclations.push_back(desc);" + nextLine(1)
		        code += tap(3) + "}" + nextLine(1)
        code += nextLine(1) + tap(2) + "}"

shaderFactory = ShaderFactory(shaderFactoryFileDir)
shaderFactory.Run(code)