"""
Reference for colormaps included with Matplotlib.

This reference example shows all colormaps included with Matplotlib. Note that
any colormap listed here can be reversed by appending "_r" (e.g., "pink_r").
These colormaps are divided into the following categories:

Sequential:
    These colormaps are approximately monochromatic colormaps varying smoothly
    between two color tones---usually from low saturation (e.g. white) to high
    saturation (e.g. a bright blue). Sequential colormaps are ideal for
    representing most scientific data since they show a clear progression from
    low-to-high values.

Diverging:
    These colormaps have a median value (usually light in color) and vary
    smoothly to two different color tones at high and low values. Diverging
    colormaps are ideal when your data has a median value that is significant
    (e.g.  0, such that positive and negative values are represented by
    different colors of the colormap).

Qualitative:
    These colormaps vary rapidly in color. Qualitative colormaps are useful for
    choosing a set of discrete colors. For example::

        color_list = plt.cm.Set3(np.linspace(0, 1, 12))

    gives a list of RGB colors that are good for plotting a series of lines on
    a dark background.

Miscellaneous:
    Colormaps that don't fit into the categories above.

"""
import numpy as np
import matplotlib.pyplot as plt

# Have colormaps separated into categories:
# http://matplotlib.org/examples/color/colormaps_reference.html

cmaps = [#('Perceptually Uniform Sequential',
         #                   ['viridis', 'inferno', 'plasma', 'magma']),
         ('Sequential',     ['Blues', 'BuGn', 'BuPu',
                             'GnBu', 'Greens', 'Greys', 'Oranges', 'OrRd',
                             'PuBu', 'PuBuGn', 'PuRd', 'Purples', 'RdPu',
                             'Reds', 'YlGn', 'YlGnBu', 'YlOrBr', 'YlOrRd']),
         ('Sequential (2)', ['afmhot', 'autumn', 'bone', 'cool',
                             'copper', 'gist_heat', 'gray', 'hot',
                             'pink', 'spring', 'summer', 'winter']),
         ('Diverging',      ['BrBG', 'bwr', 'coolwarm', 'PiYG', 'PRGn', 'PuOr',
                             'RdBu', 'RdGy', 'RdYlBu', 'RdYlGn', 'Spectral',
                             'seismic']),
         ('Qualitative',    ['Accent', 'Dark2', 'Paired', 'Pastel1',
                             'Pastel2', 'Set1', 'Set2', 'Set3']),
         ('Miscellaneous',  ['gist_earth', 'terrain', 'ocean', 'gist_stern',
                             'brg', 'CMRmap', 'cubehelix',
                             'gnuplot', 'gnuplot2', 'gist_ncar',
                             'nipy_spectral', 'jet', 'rainbow',
                             'gist_rainbow', 'hsv', 'flag', 'prism'])]


nrows = max(len(cmap_list) for cmap_category, cmap_list in cmaps)
gradient = np.linspace(0, 1, 256)
gradient = np.vstack((gradient, gradient))

def interp_subspec(pt, subspec):
    x0 = None
    y0 = None
    x1 = None
    y1 = None

    for x, y0_, y1_ in subspec:
        if x <= pt:
            x0 = x
            y0 = y0_
        if x > pt:
            x1 = x
            y1 = y1_
            break

    if x1 is None:
        x1 = subspec[-1][0]
        y1 = subspec[-1][2]

    if pt == x0: return y0
    if pt == x1: return y1

    assert(x0 is not None)
    assert(x1 is not None)
    assert(y0 is not None)
    assert(y1 is not None)
    #print map(type, [pt, y0, x0, x1, y1])
    return y0 + (y1-y0)*(pt-x0)/(x1-x0)

def check(name, result):
    for line in result:
        for i, elem in enumerate(line):
            if elem > 1.0:
                line[i] = 1.0
            if elem < 0.0:
                line[i] = 0.0

def spec_to_lin(name, spec):

    if hasattr(spec['red'], '__call__'):
        linMap = list()
        x = np.arange(0.0, 1.0+0.01, 0.01)
        reds = spec['red'](x)
        greens = spec['green'](x)
        blues = spec['blue'](x)
        if 'alpha' in spec:
            alphas = spec['alpha'](x)
        for i, x in enumerate(x):
            data = [x, reds[i], greens[i], blues[i]]
            if 'alpha' in spec:
                data.append(alphas[i])
            linMap.append(data)
        check(name, linMap)
        return linMap

    allPts = list()
    for key, subspec in spec.iteritems():
        for x, y0, y1 in subspec:
            allPts.append(x)

    linMap = list()
    for pt in sorted(set(allPts)):
        data = [pt,
            interp_subspec(pt, spec['red']),
            interp_subspec(pt, spec['green']),
            interp_subspec(pt, spec['blue'])]
        if 'alpha' in spec:
            data.append(interp_subspec(pt, spec['alpha']))
        linMap.append(data)

    check(name, linMap)
    return linMap

def plot_color_gradients(cmap_category, cmap_list):
    #fig, axes = plt.subplots(nrows=nrows)
    #fig.subplots_adjust(top=0.95, bottom=0.01, left=0.2, right=0.99)
    #axes[0].set_title(cmap_category + ' colormaps', fontsize=14)

    #for ax, name in zip(axes, cmap_list):
    for name in cmap_list:
        try:
            #ax.imshow(gradient, aspect='auto', cmap=plt.get_cmap(name))
            #pos = list(ax.get_position().bounds)
            #x_text = pos[0] - 0.01
            #y_text = pos[1] + pos[3]/2.
            #fig.text(x_text, y_text, name, va='center', ha='right', fontsize=10)
            yield name, spec_to_lin(name, plt.get_cmap(name)._segmentdata)
        except Exception as ex:
            print '#'*40,name,str(ex),plt.get_cmap(name)._segmentdata

    # Turn off *all* ticks & spines, not just the ones with colormaps.
    #for ax in axes:
    #    ax.set_axis_off()

f = open("GeneratedColorMaps.cpp", 'w')
titles = list()
names = list()

f.write('//\n')
f.write('//Machine generated color maps extracted from matplotlib cm module\n')
f.write('//\n')
f.write('\n')
f.write('#include "GeneratedColorMaps.hpp"\n')
f.write('\n')

for cmap_category, cmap_list in cmaps:
    for name, data in plot_color_gradients(cmap_category, cmap_list):
        title = (cmap_category + '/' + name).replace("_", ' ').title()
        f.write('static std::vector<std::vector<double>> make_color_map_%s(void)\n'%name)
        f.write('{\n')
        f.write('    std::vector<std::vector<double>> out;\n')
        for d in data:
            args = ','.join(map(str, d))
            f.write('    out.push_back(std::vector<double>{%s});\n'%args)
        f.write('    return out;\n')
        f.write('}\n')
        f.write('\n')
        titles.append(title)
        names.append(name)

f.write('\n')
f.write('std::vector<std::pair<const std::string, const std::string>> available_color_maps(void)')
f.write('{\n')
f.write('    std::vector<std::pair<const std::string, const std::string>> out;\n')
for title, name in zip(titles, names):
    f.write('    out.emplace_back("%s", "%s");\n'%(title, name));
f.write('    return out;\n')
f.write('}\n')
f.write('\n')
f.write('std::vector<std::vector<double>> make_color_map(const std::string &name)\n')
f.write('{\n')
for name in names:
    f.write('    if (name == "%s") return make_color_map_%s();\n'%(name, name))

f.write('    return std::vector<std::vector<double>>();\n')
f.write('}\n')


#plt.show()
