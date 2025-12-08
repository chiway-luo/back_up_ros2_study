from setuptools import find_packages, setup
from glob import glob
package_name = 'py01_launch'

setup(
    name=package_name,
    version='0.0.0',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
        #配置launch文件
        # ('share/' + package_name, ['launch/py/py01_2turtlesim_launch.py']),
        # ('share/' + package_name, ['launch/xml/xml01_2turtlesim_launch.xml']),
        # ('share/' + package_name, ['launch/yaml/yaml01_2turtlesim_launch.yaml']),
        ('share/' + package_name, glob('launch/py/*_launch.py')),
        ('share/' + package_name, glob('launch/xml/*_launch.xml')),
        ('share/' + package_name, glob('launch/yaml/*_launch.yaml')),
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
            'py01_node = py01_launch.py01_node:main'
        ],
    },
)
