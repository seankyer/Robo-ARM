# Copyright (c) 2019 Foundries.io
# Copyright (c) 2022 Nordic Semiconductor ASA
# SPDX-License-Identifier: Apache-2.0

'''example_west_command.py

Example of a west extension in the example-application repository.'''

from west.commands import WestCommand  # your extension must subclass this
from west import log                   # use this for user output
import subprocess
import time

def run_and_execute_app():
    result = subprocess.run(["west", "build", "-p", "-b", "native_sim", "app"], capture_output=True, text=True)
    log.inf(result.stdout)

    log_file = "pathfind.txt"
    timeout = 15
    start_time = time.time()

    with open(log_file, "w") as f:
        process = subprocess.Popen(["./build/zephyr/zephyr.exe"], stdout=f, stderr=f)

    while True:
        elapsed_time = time.time() - start_time
        if elapsed_time > timeout:
            log.inf("Process timed out, terminating")
            process.teminate()
            exit

        with open(log_file, "r") as f:
            if "===DONE===" in f.read():
                log.inf("Log files generated")
                process.terminate()
                break
        time.sleep(0.5)


class Pathfind(WestCommand):

    def __init__(self):
        super().__init__(
            'pathfind',               # gets stored as self.name
            'pathfinding west command',  # self.help
            # self.description:
            'Build and run Pathfinding in native_sim, capture output and generate space plots'
        )

    def do_add_parser(self, parser_adder):
        # This is a bit of boilerplate, which allows you full control over the
        # type of argparse handling you want. The "parser_adder" argument is
        # the return value of an argparse.ArgumentParser.add_subparsers() call.
        parser = parser_adder.add_parser(self.name,
                                         help=self.help,
                                         description=self.description)

        # Add some example options using the standard argparse module API.
        # parser.add_argument('-o', '--optional', help='an optional argument')
        # parser.add_argument('required', help='a required argument')

        return parser           # gets stored as self.parser

    def do_run(self, args, unknown_args):
        # This gets called when the user runs the command, e.g.:
        #
        #   $ west my-command-name -o FOO BAR
        #   --optional is FOO
        #   required is BAR
        # log.inf('--optional is', args.optional)
        # log.inf('required is', args.required)

        log.inf("Running `west pathfind`")
        run_and_execute_app()
        log.inf("Done generating pathfind.txt")
        process = subprocess.run(["python", "./scripts/generate_plots.py"], check=True)
        log.inf("Done `west pathfind`!")
