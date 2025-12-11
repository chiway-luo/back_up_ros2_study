from setuptools import find_packages, setup
from glob import glob 

package_name = 'py05_exercise'

setup(
    name=package_name,
    version='0.0.0',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
        ('share/' + package_name, glob('launch/*_launch.xml'))
        
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
            'demo01_turtle_follow_py = py05_exercise.demo01_turtle_follow_py:main',
            'exer01_spawn_py = py05_exercise.exer01_spawn_py:main',
            'exer02_tf_broadcaster_py = py05_exercise.exer02_tf_broadcaster_py:main',
            'exer03_tf_listener_py = py05_exercise.exer03_tf_listener_py:main',
        ],
    },
)
