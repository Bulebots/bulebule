import yaml
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


def explode_yaml_series(series):
    """
    Convert a Series with YAML strings into a DataFrame.
    """
    def x(row):
        return yaml.safe_load(row)

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
