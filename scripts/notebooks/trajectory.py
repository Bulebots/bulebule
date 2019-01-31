from dataclasses import dataclass

from matplotlib import pyplot
from matplotlib.patches import Rectangle
from numpy import array
from numpy import cos
from numpy import isclose
from numpy import pi
from numpy import sin
from numpy import sign
from numpy import sqrt
from numpy import tan
from numpy.linalg import norm
from pandas import DataFrame


@dataclass
class Maze:
    cell: float
    post: float


@dataclass
class RobotPhysics:
    mass: float
    moment_of_inertia: float
    width: float
    wheels_separation: float
    max_angular_velocity: float


class Line:
    def __init__(self, x, y, angle):
        self.reference = array((x, y))
        self.angle = angle
        self.slope = tan(angle)
        self.intercept = y - x * self.slope


def lines_intersection(l0, l1):
    """Calculate the intersection of two lines."""
    if isclose(l0.slope, l1.slope):
        raise ValueError('Lines are parallel!')
    x = (l1.intercept - l0.intercept) / (l0.slope - l1.slope)
    y = l0.slope * x + l0.intercept
    return array((x, y))


class TurnProfile:
    def __init__(self, profile):
        self.profile = profile

    def plot_forces(self):
        self.profile[['centrifugal_force',
                      'angular_acceleration_force',
                      'total_force']].plot(style='.-')


class SlalomTurnProfile(TurnProfile):
    def __init__(self, profile, entry, exit, maze):
        super().__init__(profile)
        self.entry = entry
        self.exit = exit
        self.maze = maze

    def describe(self):
        step = self.profile['linear_velocity'].iloc[0] * \
            self.profile['period'].iloc[0]
        xy = self.profile[['x', 'y']]
        before = norm(xy.iloc[0].values - self.entry.reference)
        before *= sign(norm(xy.iloc[1].values - self.entry.reference) - before)
        after = norm(xy.iloc[-1].values - self.exit.reference)
        after *= sign(norm(xy.iloc[-2].values - self.exit.reference) - after)
        radius = self.profile['radius'].iloc[0]
        transition = (~self.profile['arc']).sum() * step / 2
        arc = self.profile['arc'].sum() * step
        print('Distance to post: %.3f' % self.profile['margin'].min())
        print('Turn completed in %.3f seconds' % self.profile.index[-1])
        print('Linear velocity: %.3f' %
              self.profile['linear_velocity'].iloc[0])
        print('Finished at (%.5f, %.5f)' %
              tuple(self.profile[['x', 'y']].iloc[-1].values))
        print('Parameters: {%.5f, %.5f, %.5f, %.5f, %.5f, xxx}' %
              (before, after, radius, transition, arc))

    def plot_trajectory(self):
        figure, axes = pyplot.subplots(
            1, figsize=(8, 8), subplot_kw={'aspect': 'equal'})
        post = self.maze.post
        axes.add_patch(Rectangle((-post / 2, -post / 2), post, post))
        axes.plot(self.profile['x'], self.profile['y'], '.-')
        axes.plot(self.profile['left_side_x'],
                  self.profile['left_side_y'], 'r-')
        axes.plot(self.profile['right_side_x'],
                  self.profile['right_side_y'], 'r-')
        pyplot.show()


def turn_shift(entry, exit, profile):
    expected = lines_intersection(entry, exit)
    start = Line(**profile[['x', 'y']].iloc[0].to_dict(), angle=entry.angle)
    stop = Line(**profile[['x', 'y']].iloc[-1].to_dict(), angle=exit.angle)
    actual = lines_intersection(start, stop)
    return expected - actual


def turn_profile(angle, max_angular_velocity,
                 max_angular_acceleration, time_period):
    duration = max_angular_velocity / max_angular_acceleration * pi
    transition_angle = duration * max_angular_velocity / pi
    arc = (abs(angle) - 2 * transition_angle) / max_angular_velocity
    transition = duration / 2
    max_angular_velocity = max_angular_velocity * sign(angle)

    angular_velocity = [0]
    time = [0]
    while True:
        if time[-1] >= 2 * transition + arc:
            break
        new_angular_velocity = max_angular_velocity
        if time[-1] < transition:
            factor = time[-1] / transition
            new_angular_velocity *= sin(factor * pi / 2)
        elif time[-1] >= transition + arc:
            factor = (time[-1] - arc) / transition
            new_angular_velocity *= sin(factor * pi / 2)
        angular_velocity.append(new_angular_velocity)
        time.append(time[-1] + time_period)

    profile = DataFrame({'angular_velocity': angular_velocity})
    profile['arc'] = profile['angular_velocity'] == max_angular_velocity
    return profile


def complete_profile(profile, angle, radius, linear_velocity, robot,
                     time_period):
    profile['radius'] = radius
    profile['period'] = time_period
    profile['linear_velocity'] = linear_velocity
    profile['angle'] = (profile['angular_velocity'] *
                        profile['period']).cumsum() + angle
    profile['x'] = (profile['linear_velocity'] * cos(profile['angle']) *
                    profile['period']).cumsum()
    profile['y'] = (profile['linear_velocity'] * sin(profile['angle']) *
                    profile['period']).cumsum()

    profile['centrifugal_force'] = robot.mass * profile['linear_velocity'] * \
        profile['angular_velocity'] / 2
    angular_acceleration = profile['angular_velocity'].diff() / \
        profile['period']
    profile['angular_acceleration_force'] = \
        (robot.moment_of_inertia * angular_acceleration /
         robot.wheels_separation).abs()
    profile['total_force'] = sqrt(profile['centrifugal_force'] ** 2 +
                                  profile['angular_acceleration_force'] ** 2)

    profile['time'] = profile['period'].cumsum()
    profile = profile.set_index('time')
    return profile


def complete_slalom_profile(profile, entry, exit, robot, maze, shift=None):
    """
    Complete a slalom profile.

    - Calculate and apply shift according to entry/exit points
    - Calculate trajectory of the robot's outline (with the robot width)
    - Calculate distance from the outline to the post (margin)
    """
    if shift is None:
        shift = turn_shift(entry, exit, profile)
    else:
        shift += array(entry.reference)
    profile['x'] += shift[0]
    profile['y'] += shift[1]

    left_angle = profile['angle'].values + pi / 2
    cos_width = cos(left_angle) * robot.width / 2
    sin_width = sin(left_angle) * robot.width / 2
    profile['left_side_x'] = profile['x'] + cos_width
    profile['left_side_y'] = profile['y'] + sin_width
    profile['right_side_x'] = profile['x'] - cos_width
    profile['right_side_y'] = profile['y'] - sin_width

    profile['margin'] = \
        norm(profile[['left_side_x', 'left_side_y']].values, axis=1) - \
        sqrt(maze.post ** 2 / 2)
    return profile


class Simulator:
    def __init__(self, robot, maze, time_period):
        self.robot = robot
        self.maze = maze
        self.time_period = time_period

    def inplace(self, angle, force=0.25):
        angular_acceleration = \
            force * self.robot.wheels_separation / self.robot.moment_of_inertia
        max_angular_velocity = sqrt(abs(angle) / 2 * angular_acceleration)
        max_angular_velocity = min(max_angular_velocity,
                                   self.robot.max_angular_velocity)
        profile = turn_profile(angle, max_angular_velocity,
                               angular_acceleration, self.time_period)
        profile = complete_profile(profile, 0., 0., 0., self.robot,
                                   self.time_period)
        return TurnProfile(profile)

    def slalom(self, entry, exit, radius, force=0.25, shift=None):
        x, y, angle = entry
        entry = Line(x=x * self.maze.cell, y=y * self.maze.cell, angle=angle)
        x, y, angle = exit
        exit = Line(x=x * self.maze.cell, y=y * self.maze.cell, angle=angle)
        angle = exit.angle - entry.angle
        linear_velocity = sqrt(2 * force * radius / self.robot.mass)
        max_angular_velocity = linear_velocity / radius
        max_angular_velocity = min(max_angular_velocity,
                                   self.robot.max_angular_velocity)
        max_angular_acceleration = \
            force * self.robot.wheels_separation / self.robot.moment_of_inertia
        max_angular_velocity_transition = \
            sqrt(abs(angle) / 2 * max_angular_acceleration)
        if max_angular_velocity_transition < max_angular_velocity:
            raise ValueError
        profile = turn_profile(angle, max_angular_velocity,
                               max_angular_acceleration, self.time_period)
        profile = complete_profile(
            profile, entry.angle, radius, linear_velocity,
            self.robot, self.time_period
        )
        profile = complete_slalom_profile(
            profile, entry, exit, self.robot, self.maze, shift=shift)
        return SlalomTurnProfile(profile, entry, exit, self.maze)
