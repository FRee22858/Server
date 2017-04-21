import string

class CMyClass:
       def HelloWorld(self):
           print ('hellolll')

class SecondClass:
       def invoke(self,obj):
           obj.HelloWorld()

def HelloWorld(strName):
    print ("Hello "), strName

def Add(a, b, c):
    return a + b + c

def AddMult(a, b):
    print ("in FunctionAddMult...")
    a,b
    return a + b, a * b

def StringToUpper(strSrc):
    return string.upper(strSrc)