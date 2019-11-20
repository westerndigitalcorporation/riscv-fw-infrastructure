import os

class setConfigure(object):
  # set the configure
  def __init__(self):
    self.listDemos = []
    self.intDemo = -1
    self.scanDemos()
    self.pickDemo()
    self.setConfig()
    
  def scanDemos(self):
    # scan the build/demo folder and grab all the files with pattern "demo_xxxx.py
    listFiles = os.listdir(os.path.join(os.getcwd(), "demos"))
    for strFile in listFiles:
      if strFile.startswith("demo_") and strFile.endswith(".py"):
        self.listDemos.append(strFile) 
        
  def pickDemo(self):
    # list all the demos found in the build/demos folder and wait for the user to pick one
    for strFile in self.listDemos:
      strDemoName = strFile.replace("demo_", "").replace(".py", "")
      print "%s: %s" % (self.listDemos.index(strFile), strDemoName)
    
    while(True):
      strDemo = raw_input("Please select a demo:")
      if not strDemo.isdigit():
        print "Please enter the demo index!"
      elif int(strDemo) > (len(self.listDemos) - 1):
        print "Demo index out of range!"
      else:
        self.intDemo = int(strDemo)
        break
        
  def setConfig(self):
    # save the configureation in the configure file in the build folder
    strDemoName = self.listDemos[self.intDemo].replace("demo_", "").replace(".py", "")
    print strDemoName
    f  = open("configure.txt", "w")
    f.write(strDemoName)
    f.close()
    
if __name__ == "__main__":
  objConfigure= setConfigure()