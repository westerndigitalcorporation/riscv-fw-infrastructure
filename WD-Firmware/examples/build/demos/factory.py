import os
import importlib

class generate(object):
  def __init__(self, strDemo):
    self.listDemos = []
    self.intDemo = -1
    if strDemo:
      self.listDemos.append("demo_%s" % strDemo.replace("-", "_"))
      self.intDemo = 0
    else:
      self.scanDemos()
      self.pickDemo()
    
  def scanDemos(self):
    listFiles = os.listdir(os.path.join(os.getcwd(), "demos"))
    for strFile in listFiles:
      if strFile.startswith("demo_") and strFile.endswith(".py"):
        self.listDemos.append(strFile)
        
  def pickDemo(self):
    for strFile in self.listDemos:
      print "%s: %s" % (self.listDemos.index(strFile), strFile.replace("demo_", "").replace(".py", ""))
    
    while(True):
      strDemo = raw_input("Please select a demo:")
      if not strDemo.isdigit():
        print "Please enter the demo index!"
      elif int(strDemo) > (len(self.listDemos) - 1):
        print "Demo index out of range!"
      else:
        self.intDemo = int(strDemo)
        break
    
  def setDemo(self):
    strModuleName = "demos." + self.listDemos[self.intDemo].replace(".py", "")
    mdlDemo = importlib.import_module(strModuleName)
    objClass = getattr(mdlDemo, "demo")
    return mdlDemo.demo()
    