import os
import importlib

class generate(object):
  def __init__(self):
    self.strDemo = ""
    self.getConfigure()
    
  def setDemo(self):
    # if somehow the demo has not been set correctl break the build
    if not self.strDemo:
      print "No demo has been selected!"
      print "Please run the config.sh from the buil folder."
      exit(1)
    # import the demo class accordig to the configure 
    strModuleName = "demos.demo_" + self.strDemo
    mdlDemo = importlib.import_module(strModuleName)
    objClass = getattr(mdlDemo, "demo")
    return mdlDemo.demo()
    
  def getConfigure(self):
    # if he configure file does not exist break the build
    if not os.path.isfile("configure.txt"):
      print "No configure file has been found!"
      print "Please run the config.sh from the buil folder."
      exit(1)
    # read the configure file from he build folder and grab its info
    f = open("configure.txt", "r")
    self.strDemo = f.read()
    f.close()
    