
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

class PatchModules(WestCommand):
  
  def __init__(self):
    super(PatchModules, self).__init__(
      'patch-modules',
      'Patch sources of modules',
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
        parser.add_argument('-m', '--module-name', help='module to patch')

        return parser           # gets stored as self.parser

  def do_run(self, args, unknown_args):
      # print(os.environ)

      if(args.zephyr_path == None):
        args.zephyr_path = os.environ.get('ZEPHYR_BASE')

      if(args.module_name == None):
        log.die("No module set")

      args.zephyr_path += '/../modules/'
      args.zephyr_path += args.module_name

      if(args.patch_path == None):
        args.patch_path = join(os.getcwd(), "patches", "modules")

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
            log.err(join("Unable to apply patch: ", filename))
