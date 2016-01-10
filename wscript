
#
# This file is the default set of rules to compile a Pebble project.
#
# Feel free to customize this to your needs.
#

import os.path
import json
import shutil

top = '.'
out = 'build'

def options(ctx):
    ctx.load('pebble_sdk')

def configure(ctx):
    ctx.load('pebble_sdk')

def distclean(ctx):
    if os.path.exists('build'): shutil.rmtree('build')
    if os.path.exists('src/generated'): shutil.rmtree('src/generated')
    if os.path.exists('src/js/generated'): shutil.rmtree('src/js/generated')

def build(ctx):
    ctx.load('pebble_sdk')

    ctx.add_pre_fun(generate_appinfo_h)
    ctx.add_pre_fun(generate_keys_h)
    ctx.add_pre_fun(generate_keys_js)

#    ctx(rule=generate_appinfo_h, source='../appinfo.json', target='../src/generated/appinfo.h')

    build_worker = os.path.exists('worker_src')
    binaries = []

    for p in ctx.env.TARGET_PLATFORMS:
        ctx.set_env(ctx.all_envs[p])
        ctx.set_group(ctx.env.PLATFORM_NAME)
        app_elf='{}/pebble-app.elf'.format(ctx.env.BUILD_DIR)
        ctx.pbl_program(source=ctx.path.ant_glob('src/**/*.c'), target=app_elf)

        if build_worker:
            worker_elf='{}/pebble-worker.elf'.format(ctx.env.BUILD_DIR)
            binaries.append({'platform': p, 'app_elf': app_elf, 'worker_elf': worker_elf})
            ctx.pbl_worker(source=ctx.path.ant_glob('worker_src/**/*.c'), target=worker_elf)
        else:
            binaries.append({'platform': p, 'app_elf': app_elf})

    ctx.set_group('bundle')

    js_files = ctx.path.ant_glob('src/**/*.js')
    if js_files:
        ctx(rule='cat ${SRC} > ${TGT}', source=js_files, target='pebble-js-app.js')
    ctx.pbl_bundle(binaries=binaries, js='pebble-js-app.js' if js_files else [])

def generate_appinfo_h(ctx):
    print('Generating appinfo.h')
    
    src = 'appinfo.json'
    targetdir = 'src/generated/'
    target = targetdir + 'appinfo.h'
    appinfo = json.load(open(src))

    if not os.path.exists(targetdir): os.mkdir(targetdir)
    f = open(target, 'w')
    f.write('#pragma once\n\n')
    f.write('#define VERSION_LABEL "{0}"\n'.format(appinfo["versionLabel"]))
    f.write('#define UUID "{0}"\n'.format(appinfo["uuid"]))
    for key in appinfo['appKeys']:
        f.write('#define APP_KEY_{0} {1}\n'.format(key.upper(), appinfo['appKeys'][key]))
    f.close()

def generate_keys_h(ctx):
    src = 'src/keys.json'
    targetdir = 'src/generated/'
    target = targetdir + 'keys.h'
    keys = json.load(open(src))
    if not os.path.exists(targetdir): os.mkdir(targetdir)
    f = open(target, 'w')
    f.write('#pragma once\n\n')
    for key in keys:
        f.write('enum {\n')
        for i, key2 in enumerate(keys[key]):
            f.write('\tKEY_{0}_{1} = {2},\n'.format(key, key2, i))
        f.write('};\n')
    f.close()

def generate_keys_js(ctx):
    src = 'src/keys.json'
    targetdir = 'src/js/generated/'
    target = targetdir + 'keys.js'
    keys = json.load(open(src))
    if not os.path.exists(targetdir): os.mkdir(targetdir)
    f = open(target, 'w')
    for key in keys:
        f.write('var {0} = {{\n'.format(key))
        for i, key2 in enumerate(keys[key]):
            f.write('\t{0}: {1},\n'.format(key2, i))
        f.write('};\n')
    f.close()
