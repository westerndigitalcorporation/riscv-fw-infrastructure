from abstract import clsdemo_abstract

class demo(clsdemo_abstract):
  def __init__(self):
    clsdemo_abstract.__init__(self)
        
    self.public_defs = [
        'D_BARE_METAL',
    ]
      
    self.moduls = [
      'comrv_baremetal',
      'psp'
    ]