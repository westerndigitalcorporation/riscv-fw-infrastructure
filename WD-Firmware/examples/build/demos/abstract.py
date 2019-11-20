

class clsdemo_abstract(object):
  # paren class of the demo classes
  def __init__(self):
    self.name = ""
    self.rtos_core = ""
    self.file_name = ""
    self.map_file = ""
    self.elf_file = ""
    self.public_defs = []
    self.moduls = []
    self.rtos_defines = []
    
  def set_outputfiles_names(self, prefix = "", suffix = ""):
    # format the artifacts files name for the current demo
    self.file_name = self.name
    if prefix:
      self.file_name = "%s-%s" % (prefix, self.name)
    if suffix:
      self.file_name = "%s-%s" % (self.file_name, suffix)
    self.map_file = self.file_name + ".map"
    self.elf_file = self.file_name + ".elf"
    
