from west.commands import WestCommand
from west import log

from os import listdir, chdir, system, environ, getcwd
from patch import fromfile
from os.path import abspath, dirname, exists, join, isdir, isfile

class PatchZephyr(WestCommand):

  def __init__(self):
    super(PatchZephyr, self).__init__(
      'patch-zephyr',
      'Patch sources of zephyr project',
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
      # print(environ)
      here_dir = dirname(abspath(__file__))

      if not args.zephyr_path:
        args.zephyr_path = environ.get('ZEPHYR_BASE')

      if not args.patch_path:
        args.patch_path = join(here_dir, "..", "patches", "zephyr")

      save_cwd = getcwd()
      chdir(args.zephyr_path)
      for filename in sorted(listdir(args.patch_path)):
        print(f"Processing {filename} patch file.")
        patch_file = join(args.patch_path, filename)
        pto = fromfile(patch_file)
        if pto is False:
            continue
        else:
          if system(f'git apply --ignore-space-change --ignore-whitespace {patch_file}') == 0:
            log.inf(join("Patch applied correctly: ", filename))
          else:
            log.err(join("Unable to apply patch: ", filename))
