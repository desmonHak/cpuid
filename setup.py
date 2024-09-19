from distutils.core import setup, Extension

"""
 * python setup.py build_ext --inplace 
 * utiliza setup.py para crear nuestros 
 * módulos de extensión. El indicador --inplace mueve los objetos compartidos 
 * compilados al árbol de origen para que estén en la ruta de búsqueda de Python.
"""

# https://stackoverflow.com/questions/64261546/how-to-solve-error-microsoft-visual-c-14-0-or-greater-is-required-when-inst

def main():
    setup(
        name         = "cpuid_native",
        version      = "1.0.0",
        description  = "Lib CPUID",
        author       = "desmonHak",
        author_email = "",
        ext_modules  = [
            Extension(
                "cpuid_x86", [
                    "cpuid_python/cpuid_py.c"
                ],
                extra_link_args = ['cpuid.o']

            )
        ],
        classifiers=[
            'Development Status :: 3 - Alpha',
            'License :: OSI Approved :: None',
            'Natural Language :: Spanish',
            'Programming Language :: Python :: 3 :: Only',
            'Programming Language :: Python :: Implementation :: CPython',
        ]
    )

if __name__ == "__main__":
    main()