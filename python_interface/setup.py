from setuptools import setup, Extension
import glob

setup (
    name = 'hinge_framework',
    version = '1.0',
    author = "Mystic Lintha",
    description = """The Hinge Framework for Python.""",
    ext_modules=[
        Extension('_hinge_framework', 
            glob.glob('../HingeFramework/**/*.cpp', recursive=True) + 
            glob.glob('../HingeFramework/**/*.cxx', recursive=True),
            libraries=['sqlite3', 'SQLiteCpp', 'libcrypto', 'libssl'],
            include_dirs = ['D:\\VisualStudioProjects\\HingeFramework\\HingeFramework\\include'],
            library_dirs = ['D:\\VisualStudioProjects\\HingeFramework\\HingeFramework\\lib'],
            extra_compile_args=['/std:c++17']
        )
    ],
    py_modules=['hinge_framework'],  # 如果有其他Python模块也可以在这里列出
)
