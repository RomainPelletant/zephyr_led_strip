
from west.commands import WestCommand
from west import log

import sys
import os
import patch
import shutil
from os.path import abspath, dirname, exists, join, isdir, isfile
from tempfile import mkdtemp

try:
  getcwdu = os.getcwdu
except AttributeError:
  getcwdu = os.getcwd

class PatchNCS(WestCommand):
  
  def __init__(self):
    super(PatchNCS, self).__init__(
      'patch-ncs',
      'Patch sources of NCS (Nordic Connect SDK)',
      "",
      accepts_unknown_args=True)

  def do_add_parser(self, parser_adder):
        parser = parser_adder.add_parser(self.name,
                                         help=self.help,
                                         description=self.description)

        # Add some example options using the standard argparse module API.
        parser.add_argument('-p', '--patch-path', help='folder containing patches')
        parser.add_argument('-a', '--apply', action='store_true',
                            help='use "git apply" instead of "git am"')
        parser.add_argument('-z', '--zephyr-path', help='Zephyr path')

        return parser           # gets stored as self.parser

  def do_run(self, args, unknown_args):
      # print(os.environ)

      if(args.zephyr_path == None):
        args.zephyr_path = os.environ.get('ZEPHYR_BASE')

      args.zephyr_path += '/../nrf'

      if(args.patch_path == None):
        args.patch_path = join(os.getcwd(), "./patches/ncs")

      save_cwd = getcwdu()
      os.chdir(args.zephyr_path)
      for filename in os.listdir(args.patch_path):
        pto = patch.fromfile(join(args.patch_path, filename))
        if pto == False:
          continue
        else:
          if os.system('git apply --ignore-space-change --ignore-whitespace ' + join(args.patch_path, filename)) == 0:
            log.inf(join("Patch applied correctly: ", filename))
          else:
            log.die(join("Unable to apply patch: ", filename))
