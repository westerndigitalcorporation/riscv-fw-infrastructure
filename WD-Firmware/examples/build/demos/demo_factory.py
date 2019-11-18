import importlib

class generate(object):
  def __new__(self, strDemo):
        strModule =  "demos.demo_%s" % strDemo.replace("-", "_")  # [NF] - remove when grabbing from folder "-" does not apply to import
        mdlDemo = importlib.import_module(strModule)
        objClass = getattr(mdlDemo, "demo")
        return mdlDemo.demo()
        