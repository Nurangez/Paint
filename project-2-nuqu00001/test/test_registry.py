import os
import re
from shutil import which
import textwrap

def is_installed(name):
    """Check whether `name` is on PATH and marked as executable."""
    return which(name) is not None


def source_files(src_dir):
    files = []
    for file in os.listdir(src_dir):
        if (file.endswith(".c") or file.endswith(".h")) and file != "unit_tests.c" and file != "test_main.c" and "gui" not in file:
            files.append(src_dir+"/"+file)
    return files
    
def style_test(tu, test_name):
    src_dir=tu.join_base("src")
    test_bin = ("splint")
    if not is_installed(test_bin):
        return tu.FAILURE("splint binary not available")
    
    args = [
            "-nullstate", "-compmempass", "-unreachable", "-noeffect",
            "-retvalother","-retvalint","-branchstate", "-exportlocal", "-usedef", "-compdef", "-temptrans", "+charint", "-predboolint", "-predboolothers", "-nullret", "-nullderef", "-nullpass", "-unrecog", "+matchanyintegral", "-mustfreefresh", "-formatcode", "-mustdefine", "-initallelements","-boolops", "-compdestroy", "+posixlib", "-unqualifiedtrans", "-shiftnegative", 
    ]
    args += source_files(src_dir)
    
    run_res = tu.run(test_bin, args, timeout_secs=20)
    rc,outs,errs=run_res

    msg = "splint found warnings."
    ret_text=""
    if (len(outs) > 0):
        ret_text += "\nstdout:\n" + textwrap.indent(outs, "  ")
    if (len(errs) > 0):
        ret_text += "\nstderr:\n" + textwrap.indent(errs, "  ")
    if "no warning" not in ret_text:
        return tu.FAILURE(msg+ret_text)
    return tu.check(run_res, "splint test failed")

def style_test_cppcheck(tu, test_name):
    test_bin = ("cppcheck")
    if not is_installed(test_bin):
        return tu.FAILURE("cppcheck binary not available")

    run_res = tu.run("sh", ["-c", "cppcheck "+" ".join(source_files(tu.join_base("src")))])
    rc,outs,errs=run_res

    msg = "cppcheck found warnings."
    ret_text=""
    if (len(outs) > 0):
        ret_text += "\nstdout:\n" + textwrap.indent(outs, "  ")
    if (len(errs) > 0):
        ret_text += "\nstderr:\n" + textwrap.indent(errs, "  ")
    if "error:" in ret_text:
        return tu.FAILURE(msg+ret_text)
    return tu.check(run_res, "cppcheck test failed")

def style_test_cpplint(tu, test_name):
    test_bin = ("cpplint")
    if not is_installed(test_bin):
        return tu.FAILURE("cpplint binary not available")

    run_res = tu.run("sh", ["-c", "cpplint --filter=-build/header_guard,-legal/copyright,-build/include_subdir,-readability/casting,-readability/todo,-whitespace/line_length,-build/include_what_you_use "+ " ".join(source_files(tu.join_base("src")))])
    rc,outs,errs=run_res

    msg = "cpplint found warnings."
    ret_text=""
    if (len(outs) > 0):
        ret_text += "\nstdout:\n" + textwrap.indent(outs, "  ")
    if (len(errs) > 0):
        ret_text += "\nstderr:\n" + textwrap.indent(errs, "  ")
    if "Total errors found" in ret_text:
        return tu.FAILURE(msg+ret_text)
    return tu.check(run_res, "cpplint test failed")



solver_base = 'bin/convert'

def sanitize_image(img):
    img = re.sub(r'#[^\n]*\n', ' ', img)
    img = re.sub(r'\s+', ' ', img)
    img = re.sub(r' 0+', ' ', img)
    img = img.strip()
    return img

def run_test(tu, test_name):
    cat, method_category, name_params = test_name.split('.')
    name_split = name_params.split('_')
    name = name_split[0]
    params = name_split[1:]
    method = method_category + ('_' + '_'.join(params) if len(params) > 0 else '')
    input_file = tu.join_base('test/data/{1}.ppm'.format(cat, name))
    output_file = tu.join_base('test/ref_output/{1}.{2}.ppm'.format(cat, name, method))

    input = open(input_file, 'r').read()
    expected_output = open(output_file, 'r').read()
    if method.startswith('resize'):
        args = method.split('_')
    elif method.startswith('flood'):
        args = method.split('_')
    else:
        command = method.replace('_', '-')
        args = [command]

    global solver_base
    solver_bin = solver_base
    if "large" in name:
        solver_bin += "_opt"
    solver = tu.join_base(solver_bin)
    (rc, outs, errs) = tu.run(solver, args, input = input)
    for errline in errs.split('\n'):
        if len(errline) > 0:
            print('DEBUG:', errline)

    output = sanitize_image(outs)
    expected_output = sanitize_image(expected_output)
    if output == expected_output:
        return tu.SUCCESS()
    return tu.FAILURE('Incorrect output')

def run_test_broken(tu, test_name):
    cat, method, name = test_name.split('.')
    input_file = tu.join_base('test/data/{1}.ppm'.format(cat, name))

    input = open(input_file, 'r').read()

    global solver_base
    solver_bin = solver_base
    solver = tu.join_base(solver_bin)
    args = ['read-and-write']
    (rc, outs, errs) = tu.run(solver, args, input = input)
    if len(outs) > 0:
        return tu.FAILURE('Non-empty output for invalid PPM input.')
    if rc != 1:
        return tu.FAILURE('Invalid exit status. Expecting 1 for invalid PPM inputs.')
    return tu.SUCCESS()


tests = [
    'public.basi0g01',
    'public.owl',
    'public.small1',
    'public.small2',
    'public.broken-imgbroken1',
    'public.broken-imgbroken2',


]

methods = [
    'read_and_write',
    'rotate_counterclockwise',
    'rotate_clockwise',
    'mirror_horizontal',
    'mirror_vertical',
    'resize_10_12',
    'resize_100_300',
    'flood_0_0_30_120_188',
    'flood_30_25_250_155_33',
]



all_tests = {}
for test in tests:
    cat, case = test.split('.')

    if case.startswith('broken-'):
        _, case = case.split('-', 1)
        all_tests['{0}.read.{1}'.format(cat, case)] = run_test_broken
        continue

    all_methods = methods
    for method in all_methods:
        method_category = re.sub(r'\d+', '', method).rstrip('_')
        method_params = method.replace(method_category, '').lstrip('_')
        test_name = case 
        if len(method_params) > 0:
            test_name += '_' + method_params
        assert("_" not in case)
        # naming: "category.method.name_params"
        all_tests['{0}.{1}.{2}'.format(cat, method_category, test_name)] = run_test

all_tests['public.style.cpplint'] = style_test_cpplint
all_tests['public.style.splint'] = style_test


timeout_secs = 5

