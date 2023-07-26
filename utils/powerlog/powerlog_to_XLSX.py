from os import listdir
from os.path import isdir, join
from csv import reader as csv_reader
from statistics import mean, median, stdev

from matplotlib import pyplot
from xlsxwriter import Workbook

GENERATE_RAW_FILE: bool = False
GENERATE_RAW_GRAPHS: bool = False
GENERATE_COMBINED_FILE: bool = False
GENERATE_TABLES_FILE: bool = True
QUANTIZATION_STEP: int = 1  # ms

INPUT_DIR: str = r"E:\B.Sc.Arbeit\Messungen\20.07.2023"
OUTPUT_DIR: str = INPUT_DIR
OUTPUT_FILE_NAME_BASE: str = INPUT_DIR.split('\\')[-1]

RENAME_DICT: dict[str, str] = {
    "Tinkerforge/localhost:4223/244F": "ATX 3,3V",
    "Tinkerforge/localhost:4223/UgC": "ATX 5V",
    "Tinkerforge/localhost:4223/U5T": "ATX 12V",
    "Tinkerforge/localhost:4223/Ugo": "P4",
    "Tinkerforge/localhost:4223/Ugx": "P8",
    "Tinkerforge/localhost:4223/UfN": "PEG 3,3V",
    "Tinkerforge/localhost:4223/U6Q": "PEG 12V",
    "Tinkerforge/localhost:4223/244G": "PCIe 1",  # if 150W plugs were used
    "Tinkerforge/localhost:4223/244E": "PCIe 2",
    "Tinkerforge/localhost:4223/244J": "PCIe 3",
    "Tinkerforge/localhost:4223/Ugu": "PCIe 1",  # if 600W plug was used
    "Tinkerforge/localhost:4223/Ufm": "PCIe 2",
    "Tinkerforge/localhost:4223/Uft": "PCIe 3",
    "Tinkerforge/localhost:4223/UgH": "PCIe 4",
    "Tinkerforge/localhost:4223/UgF": "PCIe 5",
    "Tinkerforge/localhost:4223/UeW": "PCIe 6"
}

TABLE_CATEGORIES: list[str] = [
    "Test Setup",
    "Test Case",
    "Test Data",
    "Resolution",
    "Camera Angle",
    "Num. Frames Rendered",
    "Avg. FPS"
]

RELEVANT_POWER_SOURCES: list[str] = [
    "System without GPU", "GPU", "NVML/ADL"
]

TEST_CASE_FORMAT_JSON: dict = {"align": "left"}
TIMESTAMP_FORMAT_JSON: dict = {"num_format": "############", "align": "center"}
POWER_FORMAT_JSON: dict = {"num_format": "###,###0.000", "align": "center"}


class TestCaseMetaData:
    def __init__(self, full_name: str, num_frames_rendered: int):
        self.full_name: str = full_name
        split_info = full_name.split('|')
        self.test_case_type: str = split_info[0]
        self.test_data: str = split_info[1]
        self.resolution: int = int(split_info[2][:-1])
        self.cam_angle: str = split_info[3]
        self.num_frames_rendered: int = num_frames_rendered


if __name__ == '__main__':

    table_rows: list[list[str]] = []

    # setup .xlsx file(s)
    raw_workbook = Workbook(join(OUTPUT_DIR, OUTPUT_FILE_NAME_BASE + "-raw.xlsx"))
    combined_workbook = Workbook(join(OUTPUT_DIR, OUTPUT_FILE_NAME_BASE + "-combined_power.xlsx"))
    tables_workbook = Workbook(join(OUTPUT_DIR, OUTPUT_FILE_NAME_BASE + "-tables.xlsx"))

    # detect all subdirectories (ideally each corresponding to one GPU or similar "hard-to-modify" factor)
    test_setups: list[str] = [directory for directory in listdir(INPUT_DIR) if isdir(join(INPUT_DIR, directory))]

    for test_setup in test_setups:
        # scan all files in directory and extract 11-digit run prefixes from filenames
        run_prefixes: set[str] = set()

        for file in listdir(join(INPUT_DIR, test_setup)):
            if len(file.split('_')[0]) == 11:
                run_prefixes.add(file.split('_')[0])

        for run_prefix in run_prefixes:
            print("Processing ", str(join(INPUT_DIR, test_setup, run_prefix)))

            # collect all files of a run
            powerlog_files: list[str] = []  # contain sensor samples (timestamp + momentary power, one sensor per file)
            marker_file: str = ""  # contain markers for start/end of individual test cases

            for file in listdir(join(INPUT_DIR, test_setup)):
                if file.endswith(".csv"):
                    if file.startswith(run_prefix + "_"):
                        powerlog_files.append(file)
                    elif file.startswith(run_prefix + "-"):
                        if not (file.endswith("camera_angles.csv") or file.endswith("camera_angles_.csv")):
                            marker_file = file

            # read/parse contents of all detected files
            sensor_data: dict[str, tuple[list[int], list[float]]] = {}  # k: sensor name, v: tuple of (timestamp, power)
            marker_data: dict[str, tuple[int, int]] = {}  # key: test case name, value: (start timestamp, end timestamp)
            test_case_data: dict[str, TestCaseMetaData] = {}  # key: test case name, value: TestCaseMetaData

            for file in powerlog_files:
                with open(join(INPUT_DIR, test_setup, file), 'r') as powerlog_csv_file:
                    reader = csv_reader(powerlog_csv_file, delimiter=',')
                    data_tuples: tuple[list[int], list[float]] = ([], [])

                    for row in reader:
                        if row[2] == "inf":  # filter out unused sensors STEP 1
                            break
                        try:
                            data_tuples[0].append(int(row[1]))
                            data_tuples[1].append(float(row[2]))
                        except (IndexError, ValueError) as e:
                            if not row[1] == "Sample Timestamp (ms)":  # rule out first line
                                print(str(type(e))[8:-2] + "! File \"" + join(INPUT_DIR, test_setup, file) + "\" is invalid:")
                                print(e)

                    if len(data_tuples[0]) < 2:  # filter out unused sensors STEP 2
                        continue

                    if "Tinkerforge" in row[0] and row[0] in RENAME_DICT:
                        sensor_data[RENAME_DICT[row[0]]] = data_tuples
                    elif "ADL" in row[0]:  # potentially wrong, no successful ADL examples yet, TODO: verify / correct
                        sensor_data["ADL"] = data_tuples
                    elif "NVML" in row[0]:
                        sensor_data["NVML"] = data_tuples
                    else:
                        sensor_data[row[0]] = data_tuples

            with open(join(INPUT_DIR, test_setup, marker_file), 'r') as marker_csv_file:
                reader = csv_reader(marker_csv_file, delimiter=',')
                for row in reader:
                    try:
                        split_row = row[0].split('|')
                        if split_row[-1] == "start":  # case: test case start
                            marker_data[row[0][:-6]] = (int(row[1]), 0)
                        elif split_row[-1] == "end":  # case: test case end
                            marker_data[row[0][:-4]] = (marker_data[row[0][:-4]][0], int(row[1]))
                            test_case_data[row[0][:-4]] = TestCaseMetaData(row[0][:-4], int(row[2]))
                    except (KeyError, IndexError, ValueError) as e:
                        if not row[1] == "Sample Timestamp (ms)":  # rule out first line
                            print(str(type(e))[8:-2] + "! File \"" + join(INPUT_DIR, test_setup, marker_file) + "\" is invalid:")
                            print(e)

            # determine timestamp range for this test case
            min_timestamp: int = pow(2, 64)
            max_timestamp: int = 0

            for data in sensor_data.values():
                min_timestamp = min(min_timestamp, min(data[0]))  # find min. timestamp
                max_timestamp = max(max_timestamp, max(data[0]))  # find max. timestamp

            if GENERATE_RAW_FILE:
                # export raw, unquantized power data to .xlsx
                timestamp_format_r = raw_workbook.add_format(TIMESTAMP_FORMAT_JSON)
                power_format_r = raw_workbook.add_format(POWER_FORMAT_JSON)

                raw_worksheet = raw_workbook.add_worksheet((test_setup + ' ' + marker_file[12:-4])[0:31])
                raw_worksheet.set_paper(5)
                raw_worksheet.set_landscape()
                raw_worksheet.set_margins(0, 0, 0, 0)
                raw_worksheet.freeze_panes(1, 0)

                raw_worksheet.write(0, 0, "Timestamp (ms)")

                for i in range(min_timestamp, max_timestamp + 1):  # fill timestamp column
                    raw_worksheet.write(i - min_timestamp + 1, 0, i, timestamp_format_r)

                col: int = 1
                for sensor in sensor_data.keys():
                    raw_worksheet.write(0, col, sensor + " (W)")
                    for timestamp, power in zip(sensor_data[sensor][0], sensor_data[sensor][1]):
                        raw_worksheet.write(timestamp - min_timestamp + 1, col, power, power_format_r)
                    col += 1
                raw_worksheet.autofit()

                raw_raw_chart = raw_workbook.add_chart({"type": "scatter", "subtype": "straight_with_markers"})

                for i in range(len(sensor_data.keys())):
                    raw_raw_chart.add_series({
                        "name": [raw_worksheet.get_name(), 0, 1 + i],
                        "categories": [raw_worksheet.get_name(), 1, 0, max_timestamp - min_timestamp + 1, 0],
                        "values": [raw_worksheet.get_name(), 1, 1 + i, max_timestamp - min_timestamp + 1, 1 + i],
                        "line": {"none": True},
                        "marker": {"type": "x", "size": 2}
                    })
                    raw_raw_chart.set_x_axis({
                        "min": min_timestamp,
                        "max": max_timestamp
                    })
                    raw_raw_chart.set_size({"width": 1280, "height": 800})
                    raw_raw_chart.set_title({"name": (test_setup + ' - ' + marker_file[12:-4])[0:31]})
                    raw_raw_chart.set_legend({"position": "bottom"})

                raw_worksheet.insert_chart("L2", raw_raw_chart)
                # TODO: multiple charts (start with separating ATX/GPU)

            if GENERATE_RAW_GRAPHS:
                for sensor in sensor_data:
                    pyplot.clf()
                    pyplot.plot(sensor_data[sensor][0], sensor_data[sensor][1], "bx", markersize=2, linewidth=0.2, alpha=0.2)
                    plot_title: str = test_setup + ' ' + marker_file[12:-4] + " - " + sensor
                    pyplot.title(plot_title)
                    pyplot.xlabel("Timestamp (ms)")
                    pyplot.ylabel("Power (W)")
                    pyplot.savefig(
                        join(OUTPUT_DIR, OUTPUT_FILE_NAME_BASE + '_' + plot_title.replace(' ', '_') + ".pdf"))

            # quantize all sensor data
            quantization_range = list(range(min_timestamp, max_timestamp + QUANTIZATION_STEP, QUANTIZATION_STEP))

            for sensor in sensor_data:
                quantized_sensor_data: tuple[list[int], list[float]] = ([], [])
                raw_index: int = 0
                for i in range(len(quantization_range)):
                    if quantization_range[i] > sensor_data[sensor][0][raw_index]:
                        raw_index = min(raw_index + 1, len(sensor_data[sensor][0]) - 1)
                    quantized_sensor_data[0].append(quantization_range[i])
                    quantized_sensor_data[1].append(sensor_data[sensor][1][raw_index])
                sensor_data[sensor] = quantized_sensor_data

            # combine quantized sensor data to approximate total power consumption of GPU and System without GPU
            sensor_data["GPU"] = ([], [])  # = PEG + PCIe
            sensor_data["System without GPU"] = ([], [])  # = ATX + P4/8 - PEG

            for i in range(len(quantization_range)):
                gpu_combined_power: float = sensor_data["PEG 3,3V"][1][i] + sensor_data["PEG 12V"][1][i]
                for j in range(1, 7):
                    if "PCIe " + str(j) in sensor_data:
                        gpu_combined_power += sensor_data["PCIe " + str(j)][1][i]
                sensor_data["GPU"][0].append(quantization_range[i])
                sensor_data["GPU"][1].append(gpu_combined_power)

                system_combined_power: float = sensor_data["ATX 3,3V"][1][i] + sensor_data["ATX 5V"][1][i]
                system_combined_power += sensor_data["ATX 12V"][1][i] + sensor_data["P4"][1][i] + sensor_data["P8"][1][i]
                system_combined_power -= sensor_data["PEG 3,3V"][1][i] + sensor_data["PEG 12V"][1][i]
                sensor_data["System without GPU"][0].append(quantization_range[i])
                sensor_data["System without GPU"][1].append(system_combined_power)

            columns: list[str] = ["System without GPU", "GPU"]
            if "ADL" in sensor_data:  # potentially wrong, no successful ADL examples yet, TODO: verify / correct
                columns.append("ADL")
            if "NVML" in sensor_data:
                columns.append("NVML")

            if GENERATE_COMBINED_FILE:
                # export combined power data to .xlsx
                test_case_format = combined_workbook.add_format(TEST_CASE_FORMAT_JSON)
                timestamp_format = combined_workbook.add_format(TIMESTAMP_FORMAT_JSON)
                power_format = combined_workbook.add_format(POWER_FORMAT_JSON)

                combined_worksheet = combined_workbook.add_worksheet((test_setup + ' ' + marker_file[12:-4])[0:31])
                combined_worksheet.set_paper(5)
                combined_worksheet.set_landscape()
                combined_worksheet.set_margins(0, 0, 0, 0)
                combined_worksheet.freeze_panes(1, 0)

                combined_worksheet.write(0, 0, "Test Case")
                combined_worksheet.write(0, 1, "Timestamp (ms)")

                for col in range(len(columns)):
                    combined_worksheet.write(0, col + 2, columns[col] + " (W)")
                    row: int = 1
                    for timestamp, power in zip(sensor_data[columns[col]][0], sensor_data[columns[col]][1]):
                        if col == 0:  # timestamps are the same for all sensors (because of prior quantization)
                            for test_case in marker_data:  # assign timestamp/row to the corresponding test case, if possible
                                matching_test_case_found: bool = False
                                if marker_data[test_case][0] < timestamp < marker_data[test_case][1]:
                                    combined_worksheet.write(row, 0, test_case, test_case_format)
                                    matching_test_case_found = True
                                elif matching_test_case_found:  # values can only match one test case
                                    break
                            combined_worksheet.write(row, 1, timestamp, timestamp_format)
                        combined_worksheet.write(row, col + 2, power, power_format)
                        row += 1

                combined_worksheet.autofit()

                combined_raw_chart = combined_workbook.add_chart({"type": "scatter", "subtype": "straight_with_markers"})

                for i in range(3 if ("AML" in sensor_data or "NVML" in sensor_data) else 2):
                    combined_raw_chart.add_series({
                        "name": [combined_worksheet.get_name(), 0, 2 + i],
                        "categories": [combined_worksheet.get_name(), 1, 1, len(quantization_range), 1],
                        "values": [combined_worksheet.get_name(), 1, 2 + i, len(quantization_range), 2 + i],
                        "line": {"none": True},
                        "marker": {"type": "x", "size": 2}
                    })
                    combined_raw_chart.set_x_axis({
                        "min": sensor_data["GPU"][0][0],
                        "max": sensor_data["GPU"][0][len(sensor_data["GPU"][0]) - 1],
                    })
                    combined_raw_chart.set_size({"width": 1280, "height": 800})
                    combined_raw_chart.set_title({"name": (test_setup + ' - ' + marker_file[12:-4])})
                    combined_raw_chart.set_legend({"position": "bottom"})

                combined_worksheet.insert_chart("G2", combined_raw_chart)

            if GENERATE_TABLES_FILE:
                for test_case in test_case_data.keys():
                    test_case_runtime: float = (marker_data[test_case][1] - marker_data[test_case][0]) / 1000  # ->s
                    table_row_entries: list = [
                        test_setup,
                        test_case_data[test_case].test_case_type,
                        test_case_data[test_case].test_data,
                        test_case_data[test_case].resolution,
                        test_case_data[test_case].cam_angle,
                        test_case_data[test_case].num_frames_rendered,
                        test_case_data[test_case].num_frames_rendered / test_case_runtime,
                    ]
                    for column in columns:
                        corresponding_measurements: list[float] = []
                        for timestamp, power in zip(sensor_data[column][0], sensor_data[column][1]):
                            if marker_data[test_case][0] < timestamp < marker_data[test_case][1]:
                                corresponding_measurements.append(power)

                        table_row_entries.append(mean(corresponding_measurements))
                        table_row_entries.append(median(corresponding_measurements))
                        table_row_entries.append(min(corresponding_measurements))
                        table_row_entries.append(max(corresponding_measurements))
                        table_row_entries.append(stdev(corresponding_measurements))

                        # frames/J and J/frame relative to mean wattage
                        energy_in_joules: float = table_row_entries[-5]
                        table_row_entries.append(test_case_data[test_case].num_frames_rendered / energy_in_joules)
                        table_row_entries.append(energy_in_joules / test_case_data[test_case].num_frames_rendered)

                    table_rows.append(table_row_entries)

    if GENERATE_RAW_FILE:
        print("Writing raw file...")
        raw_workbook.close()
    if GENERATE_COMBINED_FILE:
        print("Writing combined file...")
        combined_workbook.close()
    if GENERATE_TABLES_FILE:
        test_case_format_tbl = tables_workbook.add_format(TEST_CASE_FORMAT_JSON)
        power_format_tbl = tables_workbook.add_format(POWER_FORMAT_JSON)

        tables_worksheet = tables_workbook.add_worksheet(OUTPUT_FILE_NAME_BASE + " full table")

        table_categories_formatted: list[dict[str, str]] = []
        for table_category in TABLE_CATEGORIES:
            table_categories_formatted.append({"header": table_category})
        for power_source in RELEVANT_POWER_SOURCES:
            table_categories_formatted.append({"header": '(' + power_source + ") Mean (W)"})
            table_categories_formatted.append({"header": '(' + power_source + ") Median (W)"})
            table_categories_formatted.append({"header": '(' + power_source + ") Min. (W)"})
            table_categories_formatted.append({"header": '(' + power_source + ") Max. (W)"})
            table_categories_formatted.append({"header": '(' + power_source + ") Std. Dev. (W)"})
            table_categories_formatted.append({"header": '(' + power_source + ") Efficiency (frames/J)"})
            table_categories_formatted.append({"header": '(' + power_source + ") Efficiency (J/frame)"})

        tables_worksheet.add_table(0, 0, len(table_rows), len(table_categories_formatted) - 1,
                                   {'columns': table_categories_formatted})

        for i in range(len(table_rows)):
            for j in range(len(table_rows[i])):
                if j < 6:  # check if number format should be applied
                    tables_worksheet.write(i + 1, j, table_rows[i][j], test_case_format_tbl)
                else:
                    tables_worksheet.write(i + 1, j, float(table_rows[i][j]), power_format_tbl)

        tables_worksheet.autofit()

        print("Writing tables file...")
        tables_workbook.close()
