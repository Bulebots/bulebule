from pandas import DataFrame
from pandas import Series


LOG_COLUMNS = ['timestamp', 'level', 'source', 'function', 'data']


def log_as_dataframe(log):
    """
    Convert the log list into a DataFrame.
    """
    columns = LOG_COLUMNS
    df = DataFrame(log, columns=columns)
    return df.set_index('timestamp').sort_index()


def explode_csv_series(series):
    """
    Convert a Series with CSV strings into a DataFrame.

    It assumes all CSV in the strings are floating point numbers. If an
    integer is found instead, it will be converted to a float.
    """
    def x(row):
        return [float(x) for x in row.split(',')]

    result = series.apply(x).apply(Series)
    if isinstance(result, Series):
        return DataFrame(result)
    return result


def filter_dataframe(df, dictionary):
    """
    Filter a DataFrame by the keys and values of a dictionary.

    Each key searchs on a column for the corresponding value. When setting
    multiple filters, all of them are expected to match.
    """
    for key, value in dictionary.items():
        df = df[df[key] == value]
    return df
