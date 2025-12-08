from setuptools import find_packages, setup

package_name = 'py04_param'

setup(
    name=package_name,
    version='0.0.0',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='chiway',
    maintainer_email='chiway@todo.todo',
    description='TODO: Package description',
    license='TODO: License declaration',
    extras_require={
        'test': [
            'pytest',
        ],
    },
    entry_points={
        'console_scripts': [
            'demo00_param_py = py04_param.demo00_param_py:main',
            'demo01_param_server_py = py04_param.demo01_param_server_py:main',
            'demo02_param_client_py = py04_param.demo02_param_client_py:main'
        ],
    },
)
