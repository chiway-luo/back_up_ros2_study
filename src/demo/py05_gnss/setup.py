from setuptools import find_packages, setup
from glob import glob

package_name = 'py05_gnss'

setup(
    name=package_name,
    version='0.0.0',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
        ('share/' + package_name + "/launch", glob('launch/*launch.py')),
        ('share/' + package_name + "/config", glob('config/*')),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='chiway',
    maintainer_email='chiwayfpv@gmail.com',
    description='TODO: Package description',
    license='TODO: License declaration',
    extras_require={
        'test': [
            'pytest',
        ],
    },
    entry_points={
        'console_scripts': [
            'gnss2path_py = py05_gnss.gnss2path_py:main'
        ],
    },
)
