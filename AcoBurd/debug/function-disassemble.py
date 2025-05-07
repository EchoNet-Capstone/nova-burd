import os
Import("env")

def on_after_build(source, target, env):
    script = os.path.join(env['PROJECT_DIR'], "debug", "function-disassemble.sh")
    build_dir = env['BUILD_DIR'] 
    env.Execute(f"bash {script} {build_dir}")

# Attach to the ELF link target
env.AddPostAction("$BUILD_DIR/${PROGNAME}.elf", on_after_build)